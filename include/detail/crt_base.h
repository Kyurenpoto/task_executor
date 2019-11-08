#pragma once

#include <deque>
#include <vector>
#include <algorithm>

#include "crt_mem.h"

namespace task_executor
{
    struct hazard_node_t final
    {
        void* ptr = nullptr;
        std::atomic<hazard_node_t*> next = nullptr;
        std::atomic_bool active = false;
        mem_pool_t& resource;

        hazard_node_t(mem_pool_t& rsc) :
            resource{ rsc }
        {}
    };

    struct hazard_list_t final
    {
        std::atomic<hazard_node_t*> head = nullptr;

        ~hazard_list_t()
        {
            while (true)
            {
                hazard_node_t* oldHead = head.load();
                if (oldHead == nullptr)
                    break;

                if (head.compare_exchange_weak(oldHead, oldHead->next))
                {
                    mem_pool_t& resource = oldHead->resource;
                    xdelete<hazard_node_t>(oldHead, resource);
                }
            }
        }

        hazard_node_t* alloc()
        {
            for (auto i = head.load(); i != nullptr; i = i->next.load())
            {
                bool oldActive = i->active.load();
                if (!oldActive && i->active.compare_exchange_weak(oldActive, true))
                    return i;
            }

            hazard_node_t* newHead = xnew<hazard_node_t>(getLMemPool());
            newHead->active.store(true);

            hazard_node_t* oldHead;
            do
            {
                oldHead = head.load();
                newHead->next.store(oldHead);
            } while (!head.compare_exchange_weak(oldHead, newHead));

            return newHead;
        }

        void release(hazard_node_t* node)
        {
            node->ptr = nullptr;
            node->active.store(false);
        }

        void releaseAll()
        {
            for (auto i = head.load(); i != nullptr; i = i->next.load())
                release(i);
        }

        void getHazards(std::pmr::vector<void*>& hazards)
        {
            for (auto i = head.load(); i != nullptr; i = i->next.load())
                if (i->ptr != nullptr)
                    hazards.push_back(i->ptr);

            std::sort(hazards.begin(), hazards.end(), std::less<void*>{});
        }
    };

    struct retire_node_base_t
    {
        virtual ~retire_node_base_t() = default;
        virtual void* load() = 0;
        virtual void release() = 0;
    };

    template<class T>
    struct retire_node_t final : public retire_node_base_t
    {
        T* ptr;
        mem_pool_t& resource;

        retire_node_t(T* p, mem_pool_t& rsc) :
            ptr{ p },
            resource{ rsc }
        {}

        ~retire_node_t()
        {
            release();
        }

        void* load() override
        {
            return ptr;
        }

        void release() override
        {
            xdelete<T>(ptr, resource);
        }
    };

    struct retire_list_t final
    {
        static constexpr size_t LIMIT_RETIRE = 16;

        std::pmr::deque<retire_node_base_t*> list;
        hazard_list_t& hazardList;
        mem_pool_t& resource;
        std::atomic<retire_list_t*> next = nullptr;

        retire_list_t(hazard_list_t& hazards, mem_pool_t& rsc) :
            hazardList{ hazards },
            resource{ rsc }
        {}

        ~retire_list_t()
        {
            scan();
        }

        template<class T>
        void push(T* ptr, mem_pool_t& resource)
        {
            list.push_back(static_cast<retire_node_base_t*>(
                xnew<retire_node_t<T>>(ptr, resource)));

            if (list.size() >= retire_list_t::LIMIT_RETIRE)
                scan();
        }

        void scan()
        {
            std::pmr::vector<void*> hazards;
            hazardList.getHazards(hazards);

            list.erase(
                std::remove_if(list.begin(), list.end(), [&hazards](retire_node_base_t* node) {
                    return !std::binary_search(hazards.begin(), hazards.end(), node->load()); }),
                list.end());
        }
    };

    struct retire_list_manager_t final
    {
        std::atomic<retire_list_t*> head = nullptr;

        ~retire_list_manager_t()
        {
            while (true)
            {
                retire_list_t* oldHead = head.load();
                if (oldHead == nullptr)
                    break;

                if (head.compare_exchange_weak(oldHead, oldHead->next))
                {
                    mem_pool_t& resource = oldHead->resource;
                    xdelete<retire_list_t>(oldHead, resource);
                }
            }
        }

        void push(retire_list_t* retireList)
        {
            retire_list_t* oldHead;
            do
            {
                oldHead = head.load();
                retireList->next.store(oldHead);
            } while (!head.compare_exchange_weak(oldHead, retireList));
        }
    };

    struct hazard_pointer_manager_t
    {
        hazard_list_t hazardList;
        retire_list_manager_t retireListManager;

        ~hazard_pointer_manager_t()
        {
            hazardList.releaseAll();
            retireListManager.~retire_list_manager_t();
            hazardList.~hazard_list_t();
        }
    };

    namespace detail
    {
        hazard_pointer_manager_t& getHazardPtrManager()
        {
            static hazard_pointer_manager_t hazardPtrManager;

            return hazardPtrManager;
        }

        retire_list_t& getRetireList()
        {
            static retire_list_t* retireList = nullptr;

            if (retireList == nullptr)
            {
                auto& hazardList = getHazardPtrManager().hazardList;
                auto& retireListManager = getHazardPtrManager().retireListManager;

                retireList = xnew<retire_list_t>(hazardList, getLMemPool());
                retireListManager.push(retireList);
            }

            return *retireList;
        }
    }

    hazard_node_t* allocHazardPtr()
    {
        return detail::getHazardPtrManager().hazardList.alloc();
    }

    void release(hazard_node_t* node)
    {
        detail::getHazardPtrManager().hazardList.release(node);
    }

    template<class T>
    void retire(T* ptr, mem_pool_t& resource)
    {
        detail::getRetireList().push(ptr, resource);
    }
}
