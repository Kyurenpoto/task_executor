#pragma once

#include <atomic>
#include <unordered_set>
#include <memory_resource>
#include <vector>

namespace task_executor
{
    inline namespace hazard_pointer_v1
    {
        struct hazard_pointer_manager final
        {
            hazard_pointer_manager(const hazard_pointer_manager &) = delete;
            hazard_pointer_manager(hazard_pointer_manager &&) = delete;
            hazard_pointer_manager & operator= (const hazard_pointer_manager &) = delete;
            hazard_pointer_manager & operator= (hazard_pointer_manager &&) = delete;

            hazard_pointer_manager() = default;
            ~hazard_pointer_manager() = default;

            struct hazard_list_ptr final
            {
                std::atomic_bool active = false;
                void * node = nullptr;
                hazard_list_ptr * next = nullptr;
            };

            static hazard_list_ptr * alloc()
            {
                return hazardList.alloc();
            }

            static void release(hazard_list_ptr * p)
            {
                hazardList.release(p);
            }

            template<class T>
            static void retire(T * p)
            {
                retire_node<T> * ptr = new retire_node<T>;
                ptr->store(p);
                getRetireList()->push(ptr);

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
                virtual void store(void *) = 0;
                virtual void * load() = 0;
                virtual void release() = 0;
            };

            template<class T>
            struct retire_node final :
                retire_node_base
            {
                retire_node(T * p) :
                    ptr{ p }
                {}

                ~retire_node()
                {
                    release();
                }

                void store(void * p) override
                {
                    ptr = p;
                }

                void * load() override
                {
                    return ptr;
                }

                void release() override
                {
                    delete ptr;
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
                void push(retire_node_base * node)
                {
                    retire_list_ptr * p = new retire_list_ptr;
                    
                    p->node = node;
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
                retire_list_list(const retire_list_list &) = delete;
                retire_list_list(retire_list_list &&) = delete;
                retire_list_list & operator= (const retire_list_list &) = delete;
                retire_list_list & operator= (retire_list_list &&) = delete;

                retire_list_list() = default;
                
                ~retire_list_list()
                {
                    retire_list * list;

                    while ((list = pop()) != nullptr)
                    {
                        void * p;

                        while ((p = list->pop()) != nullptr)
                            delete p;

                        delete list;
                    }
                }

                void push(retire_list * node)
                {
                    retire_list_list_ptr * p = new retire_list_list_ptr;
                    p->node = node;

                    retire_list_list_ptr * oldHead;
                    do
                    {
                        oldHead = head.load();
                        p->next = oldHead;
                    } while (!head.compare_exchange_weak(oldHead, p));
                }

                retire_list * pop()
                {
                    if (head.load() == nullptr)
                        return nullptr;

                    retire_list * node = head.load()->node;
                    
                    retire_list_list_ptr * tmp = head.load();

                    head.store(head.load()->next);

                    delete tmp;

                    return node;
                }

                std::atomic<retire_list_list_ptr *> head = nullptr;
            };

            static retire_list * getRetireList()
            {
                static thread_local retire_list * retireList = nullptr;

                if (retireList == nullptr)
                {
                    retireList = new retire_list;

                    retireListList.push(retireList);
                }

                return retireList;
            }

            static void scan()
            {
                std::unordered_multiset<void *> hazardPointers;
                for (hazard_list_ptr * p = hazardList.head.load();
                    p != nullptr; p = p->next)
                    if (p->node != nullptr)
                        hazardPointers.insert(p->node);

                for (auto p = getRetireList()->head; p != nullptr;)
                {
                    if (hazardPointers.find(p->node->load()) == hazardPointers.end())
                    {
                        delete p->node;
                        auto tmp = p->next;
                        delete p;
                        p = tmp;

                        if (getRetireList()->head == nullptr)
                            getRetireList()->head = p;
                    }
                    else
                        p = p->next;
                }
            }

            static hazard_list hazardList;
            static retire_list_list retireListList;
        };
    }
}
