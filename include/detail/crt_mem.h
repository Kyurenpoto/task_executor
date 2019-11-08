#pragma once

#include <atomic>
#include <memory_resource>
#include <utility>

namespace task_executor
{
    struct mem_pool_t final
    {
        std::pmr::unsynchronized_pool_resource resource;
        std::atomic<mem_pool_t*> next = nullptr;

        template<class T>
        T* allocate()
        {
            return static_cast<T*>(resource.allocate(sizeof(T), alignof(T)));
        }

        template<class T>
        void deallocate(T* ptr)
        {
            resource.deallocate(ptr, sizeof(T), alignof(T));
        }
    };

    struct mem_pool_manager_t final
    {
        std::atomic<mem_pool_t*> head = nullptr;

        ~mem_pool_manager_t()
        {
            while (true)
            {
                mem_pool_t* oldHead = head.load();
                if (oldHead == nullptr)
                    break;

                if (head.compare_exchange_weak(oldHead, oldHead->next))
                    delete oldHead;
            }
        }
    };

    namespace detail
    {
        mem_pool_manager_t& getGMemPoolManager()
        {
            static mem_pool_manager_t manager;

            return manager;
        }
    }

    mem_pool_t& getLMemPool()
    {
        static mem_pool_t* resource = nullptr;

        if (resource == nullptr)
        {
            resource = new mem_pool_t;

            mem_pool_manager_t& manager = detail::getGMemPoolManager();

            mem_pool_t* oldHead;
            do
            {
                oldHead = manager.head.load();
                resource->next.store(oldHead);
            } while (!manager.head.compare_exchange_weak(oldHead, resource));
        }

        return *resource;
    }

    template<class T, class... Args>
    T* xnew(Args&&... args)
    {
        mem_pool_t& resource = getLMemPool();
        void* tmp = resource.allocate<T>();

        return new(tmp) T{ std::forward<Args>(args)... };
    }

    template<class T>
    void xdelete(T* ptr, mem_pool_t& resource)
    {
        ptr->~T();
        resource.deallocate<T>(ptr);
    }

    template<class T>
    void xdelete(T* ptr)
    {
        xdelete(ptr, getLMemPool());
    }
}
