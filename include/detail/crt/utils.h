#pragma once

#include <atomic>
#include <memory_resource>

namespace task_executor
{
    inline namespace utils_v1
    {
        template<class T>
        struct atomic_monotic_list
        {
            ~atomic_monotic_list()
            {
                for (;;)
                {
                    atomic_monotic_ptr * p = head.load();
                    if (p == nullptr)
                        break;

                    head.store(p->next);

                    if (p->data == nullptr)
                        break;

                    delete p->data;
                    delete p;
                }
            }

            void push(T* data)
            {
                atomic_monotic_ptr * p = new atomic_monotic_ptr;
                p->data = data;

                atomic_monotic_ptr* oldHead;
                do
                {
                    oldHead = head.load();
                    p->next = oldHead;
                } while (!head.compare_exchange_weak(oldHead, p));
            }

        private:
            struct atomic_monotic_ptr
            {
                T * data = nullptr;
                atomic_monotic_ptr * next = nullptr;
            };

            std::atomic<atomic_monotic_ptr *> head = nullptr;
        };

        inline auto & getMemoryResourceList()
        {
            static atomic_monotic_list<std::pmr::unsynchronized_pool_resource> resourceList;

            return resourceList;
        }

        inline auto getMemoryResource()
        {
            static thread_local std::pmr::unsynchronized_pool_resource * resource = nullptr;

            if (resource == nullptr)
            {
                resource = new std::pmr::unsynchronized_pool_resource;

                getMemoryResourceList().push(resource);
            }

            return resource;
        }

        template<class T, class... Args>
        inline T * xnew(std::pmr::memory_resource & resource, Args && ... args)
        {
            T * tmp = static_cast<T *>(resource.allocate(sizeof(T), alignof(T)));

            return new(tmp) T{ std::forward(args)... };
        }

        template<class T, class... Args>
        inline T * xnew(Args && ... args)
        {
            return xnew(*getMemoryResource(), std::forward(args)...);
        }

        template<class T>
        inline void xdelete(std::pmr::memory_resource & resource, T * p)
        {
            p->~T();

            resource.deallocate(p, sizeof(T), alignof(T));
        }

        template<class T>
        inline void xdelete(T * p)
        {
            xdelete(*getMemoryResource(), p);
        }
    }
}
