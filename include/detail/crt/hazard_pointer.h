#pragma once

#include <atomic>
#include <unordered_set>

namespace task_executor
{
    inline namespace hazard_pointer_v1
    {
        struct hazard_pointer_manager
        {
            hazard_pointer_manager(const hazard_pointer_manager &) = delete;
            hazard_pointer_manager(hazard_pointer_manager &&) = delete;
            hazard_pointer_manager & operator= (const hazard_pointer_manager &) = delete;
            hazard_pointer_manager & operator= (hazard_pointer_manager &&) = delete;

            hazard_pointer_manager()
            {
                retireListList.push(&retireList);
            }

            ~hazard_pointer_manager()
            {
                retire_list * list;
                
                while ((list = retireListList.pop()) != nullptr)
                {
                    void * p;

                    while ((p = list->pop()) != nullptr)
                        delete p;
                }
            }

            struct hazard_list_ptr
            {
                std::atomic_bool active = false;
                void * node = nullptr;
                hazard_list_ptr * next = nullptr;
            };

            hazard_list_ptr * alloc()
            {
                return hazardList.alloc();
            }

            void release(hazard_list_ptr * p)
            {
                hazardList.release(p);
            }

            void retire(void * p)
            {
                retireList.push(p);

                if (retireList.size >= retire_list::MAX_SIZE)
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

            struct retire_list_ptr
            {
                void * node = nullptr;
                retire_list_ptr * next = nullptr;
            };

            struct retire_list
            {
                void push(void * node)
                {
                    retire_list_ptr * p{ new retire_list_ptr };
                    
                    p->node = node;
                    p->next = head;
                    
                    head = p;

                    ++size;
                }

                void * pop()
                {
                    if (head == nullptr)
                        return nullptr;

                    void * node = head->node;
                    
                    retire_list_ptr * tmp = head;
                    head = head->next;
                    delete tmp;

                    --size;

                    return node;
                }

                retire_list_ptr * head = nullptr;
                std::size_t size;

                static constexpr std::size_t MAX_SIZE = 128;
            };

            struct retire_list_list_ptr
            {
                retire_list * node = nullptr;
                retire_list_list_ptr * next = nullptr;
            };

            struct retire_list_list
            {
                void push(retire_list * node)
                {
                    retire_list_list_ptr * p{ new retire_list_list_ptr };
                    p->node = node;
                    p->next = head;
                    head = p;
                }

                retire_list * pop()
                {
                    if (head == nullptr)
                        return nullptr;

                    retire_list * node = head->node;
                    
                    retire_list_list_ptr * tmp = head;

                    head = head->next;

                    delete tmp;

                    return node;
                }

                retire_list_list_ptr * head = nullptr;
            };

            void scan()
            {
                std::unordered_set<void *> hazardPointers;
                for (hazard_list_ptr * p = hazardList.head.load();
                    p != nullptr; p = p->next)
                    if (p->node != nullptr)
                        hazardPointers.insert(p->node);

                for (auto p = retireList.head; p != nullptr;)
                {
                    if (hazardPointers.find(p->node) == hazardPointers.end())
                    {
                        delete p->node;
                        auto tmp = p->next;
                        delete p;
                        p = tmp;

                        if (retireList.head == nullptr)
                            retireList.head = p;
                    }
                    else
                        p = p->next;
                }
            }

            static hazard_list hazardList;
            static thread_local retire_list retireList;
            static retire_list_list retireListList;
        };
    }
}
