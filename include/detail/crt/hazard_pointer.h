#pragma once

#include <algorithm>
#include <unordered_set>

#include "utils.h"

namespace task_executor
{
    inline namespace hazard_pointer_v1
    {
        struct hazard_pointer_manager final
        {
            using hazard_list_ptr =
                atomic_reusable_list<void>::atomic_reusable_ptr;

            hazard_list_ptr * alloc()
            {
                return getHazardList().gain();
            }

            void release(hazard_list_ptr * p)
            {
                getHazardList().release(p);
                p->data = nullptr;
            }

            template<class T>
            void retire(T * p)
            {
                getRetireList()->push(p);

                if (getRetireList()->size >= retire_list::MAX_SIZE)
                    scan();
            }

            template<class T>
            void erase(T * p)
            {
                getEraseList()->push(p);

                if (getEraseList()->size >= erase_list::MAX_SIZE)
                    scan();
            }

        private:
            using hazard_list = atomic_reusable_list<void>;

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

                retire_list_ptr * remove(retire_list_ptr * p)
                {
                    retire_list_ptr * tmp = p->next;

                    p->node->release(resource);

                    p->node->~retire_node_base();
                    resource->deallocate(p->node, p->node->size, p->node->align);

                    xdelete(*resource, p);

                    if (head == nullptr)
                        head = tmp;

                    --size;
                    
                    return tmp;
                }

                static constexpr std::size_t MAX_SIZE = 128;

                retire_list_ptr * head = nullptr;
                std::size_t size = 0;
                std::pmr::unsynchronized_pool_resource * resource = nullptr;
            };

            struct erase_node_base
            {
                virtual ~erase_node_base() = default;
                virtual void * load() = 0;
                virtual void release() = 0;

                std::size_t size = 0;
                std::size_t align = 0;
            };

            template<class T>
            struct erase_node final :
                erase_node_base
            {
                erase_node(T * p) :
                    ptr{ p },
                    size{ sizeof(erase_node<T>) }
                    align{ alignof(erase_node<T>) }
                {}

                void * load() override
                {
                    return ptr;
                }

                void release() override
                {
                    ptr->~T();
                }

                T * ptr = nullptr;
            };

            struct erase_list_ptr
            {
                std::byte * loc = nullptr;
                std::size_t sizeFill = 0;
                erase_node_base * node = nullptr;
                erase_list_ptr * next = nullptr;
            };

            struct erase_list
            {
                erase_list() :
                    resource{ getMemoryResource() }
                {}

                ~erase_list()
                {
                    erase_node_base * p;

                    while ((p = pop()) != nullptr)
                    {
                        p->release();
                        p->~erase_node_base();
                        resource->deallocate(p, p->size, p->align);
                    }
                }

                template<class T>
                void push(T * ptr)
                {
                    erase_list_ptr * p = xnew<erase_list_ptr>(*resource);
                    p->node = xnew<erase_node<T>>(*resource, ptr);
                    p->next = head;
                    
                    head = p;

                    ++size;
                }

                erase_node_base * pop()
                {
                    if (head == nullptr)
                        return nullptr;

                    erase_node_base * node = head->node;
                    
                    erase_list_ptr * tmp = head;
                    head = head->next;
                    xdelete(*resource, tmp);

                    --size;

                    return node;
                }

                erase_list_ptr * remove(erase_list_ptr * p)
                {
                    erase_list_ptr * tmp = p->next;

                    std::fill(p->loc, p->loc + p->sizeFill, std::byte{ 0 });

                    xdelete(*resource, p);

                    if (head == nullptr)
                        head = tmp;

                    --size;

                    return tmp;
                }

                static constexpr std::size_t MAX_SIZE = 128;

                erase_list_ptr * head = nullptr;
                std::size_t size = 0;
                std::pmr::memory_resource * resource = nullptr;
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

            auto & getEraseListList()
            {
                static atomic_monotic_list<erase_list> eraseListList;

                return eraseListList;
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

            erase_list * getEraseList()
            {
                static thread_local erase_list * eraseList = nullptr;

                if (eraseList == nullptr)
                {
                    eraseList = new erase_list;

                    getEraseListList().push(eraseList);
                }

                return eraseList;
            }

            void scan()
            {
                std::unordered_multiset<void *> hazardPointers;
                for (auto p = getHazardList().head.load();
                    p != nullptr; p = p->next)
                    if (p->data != nullptr)
                        hazardPointers.insert(p->data);

                auto eraseList = getEraseList();

                for (auto p = eraseList->head; p != nullptr; p =
                    hazardPointers.find(p->node->load()) ==
                    hazardPointers.end() ?
                    eraseList->remove(p) :
                    p->next);

                auto retireList = getRetireList();

                for (auto p = retireList->head; p != nullptr; p =
                    hazardPointers.find(p->node->load()) ==
                    hazardPointers.end() ?
                    retireList->remove(p) :
                    p->next);
            }
        };
    }
}
