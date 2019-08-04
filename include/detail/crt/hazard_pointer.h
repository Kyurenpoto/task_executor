#pragma once

#include <unordered_set>
#include <vector>

#include "utils.h"

namespace task_executor
{
    inline namespace hazard_pointer_v1
    {
        struct hazard_pointer_manager final
        {
            struct hazard_list_ptr final
            {
                std::atomic_bool active = false;
                void * node = nullptr;
                hazard_list_ptr * next = nullptr;
            };

            hazard_list_ptr * alloc()
            {
                return getHazardList().alloc();
            }

            void release(hazard_list_ptr * p)
            {
                getHazardList().release(p);
            }

            template<class T>
            void retire(T * p)
            {
                getRetireList()->push(p);

                if (getRetireList()->size >= retire_list::MAX_SIZE)
                    scan();
            }

        private:
            struct hazard_list
            {
                ~hazard_list()
                {
                    hazard_list_ptr * p;
                    while ((p = head.load()) != nullptr)
                    {
                        head.store(p->next);
                        
                        if (p->node != nullptr)
                            delete p->node;

                        delete p;
                    }
                }

                hazard_list_ptr * alloc()
                {
                    hazard_list_ptr * p;

                    for (p = head.load(); p != nullptr; p = p->next)
                    {
                        bool tmp = p->active.load();
                        if (!tmp && p->active.compare_exchange_weak(tmp, true))
                            return p;
                    }

                    p = new hazard_list_ptr;
                    p->active.store(true);

                    hazard_list_ptr * oldHead;

                    do
                    {
                        oldHead = head.load();
                        p->next = oldHead;
                    } while (!head.compare_exchange_weak(oldHead, p));

                    return p;
                }

                void release(hazard_list_ptr * p)
                {
                    p->active.store(false);
                    p->node = nullptr;
                }

                std::atomic<hazard_list_ptr *> head = nullptr;
            };

            struct retire_node_base
            {
                virtual ~retire_node_base() = default;
                virtual void * load() = 0;
                virtual void release(std::pmr::unsynchronized_pool_resource * resource) = 0;

                std::size_t size = 0;
                std::size_t align = 0;
            };

            template<class T>
            struct retire_node final :
                retire_node_base
            {
                retire_node(T * p) :
                    ptr{ p },
                    size{ sizeof(retire_node<T>) }
                    align{ alignof(retire_node<T>) }
                {}

                void * load() override
                {
                    return ptr;
                }

                void release(std::pmr::unsynchronized_pool_resource * resource) override
                {
                    ptr->~T();

                    resource->deallocate(ptr, sizeof(T), alignof(T));
                }

                T * ptr = nullptr;
            };

            struct retire_list_ptr
            {
                retire_node_base * node = nullptr;
                retire_list_ptr * next = nullptr;
            };

            struct retire_list
            {
                retire_list() :
                    resource{ getMemoryResource() }
                {}

                ~retire_list()
                {
                    retire_node_base * p;

                    while ((p = pop()) != nullptr)
                    {
                        p->release(resource);

                        p->~retire_node_base();
                        resource->deallocate(p, p->size, p->align);
                    }
                }

                template<class T>
                void push(T * ptr)
                {
                    retire_list_ptr * p = xnew<retire_list_ptr>(*resource);
                    
                    p->node = xnew<retire_node<T>>(*resource, ptr);
                    p->next = head;
                    
                    head = p;

                    ++size;
                }

                retire_node_base * pop()
                {
                    if (head == nullptr)
                        return nullptr;

                    retire_node_base * node = head->node;
                    
                    retire_list_ptr * tmp = head;
                    head = head->next;
                    xdelete(*resource, tmp);

                    --size;

                    return node;
                }

                retire_list_ptr * erase(retire_list_ptr * p)
                {
                    auto tmp = p->next;

                    p->node->release(resource);

                    p->node->~retire_node_base();
                    resource->deallocate(p->node, p->node->size, p->node->align);

                    xdelete(*resource, p);

                    if (head == nullptr)
                        head = tmp;

                    --size;
                    
                    return tmp;
                }

                retire_list_ptr * head = nullptr;
                std::size_t size = 0;
                std::pmr::unsynchronized_pool_resource * resource = nullptr;

                static constexpr std::size_t MAX_SIZE = 128;
            };

            hazard_list & getHazardList()
            {
                static hazard_list hazardList;

                return hazardList;
            }

            auto & getRetireListList()
            {
                static atomic_monotic_list<retire_list> retireListList;

                return retireListList;
            }

            retire_list * getRetireList()
            {
                static thread_local retire_list * retireList = nullptr;

                if (retireList == nullptr)
                {
                    retireList = new retire_list;

                    getRetireListList().push(retireList);
                }

                return retireList;
            }

            void scan()
            {
                std::unordered_multiset<void *> hazardPointers;
                for (hazard_list_ptr * p = getHazardList().head.load();
                    p != nullptr; p = p->next)
                    if (p->node != nullptr)
                        hazardPointers.insert(p->node);

                for (auto p = getRetireList()->head; p != nullptr; p =
                    hazardPointers.find(p->node->load()) ==
                    hazardPointers.end() ?
                    getRetireList()->erase(p) :
                    p->next);
            }
        };
    }
}
