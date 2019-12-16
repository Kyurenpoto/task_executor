#pragma once

#include <atomic>
#include <memory>
#include <memory_resource>

namespace task_executor
{
    struct memory_pool_t
    {
        memory_pool_t* next = nullptr;
        std::atomic_bool isActive = false;
        
        struct deleter
        {
            memory_pool_t* pool = nullptr;

            template<class T>
            void operator()(T* ptr)
            {
                pool->xdelete(ptr);
            }
        };

        void restore()
        {
            bool oldIsActive;
            do
            {
                oldIsActive = isActive.load();
            } while (oldIsActive &&
                isActive.compare_exchange_weak(oldIsActive, false));
        }

        template<class T, class... Args>
        std::unique_ptr<T> xnew(Args&&... args)
        {
            void* ptr = resource.allocate(sizeof(T), alignof(T));

            if (ptr == nullptr)
                return nullptr;

            return std::unique_ptr<T>{
                new(ptr) T{ args... }, deleter{ .pool = this }};
        }

        template<class T>
        void xdelete(T* ptr)
        {
            resource.deallocate(ptr, sizeof(T), alignof(T));
        }

    private:
        std::pmr::unsynchronized_pool_resource resource;
    };

    struct memory_manager_t
    {
        ~memory_manager_t()
        {
            for (auto pool = head.load(); pool != nullptr; pool = head.load())
            {
                head.store(pool->next);

                delete pool;
            }
        }

        memory_pool_t* obtain()
        {
            for (auto pool = head.load(); pool != nullptr; pool = pool->next)
            {
                bool oldIsActive;
                do
                {
                    oldIsActive = pool->isActive.load();
                } while (!oldIsActive &&
                    pool->isActive.compare_exchange_weak(oldIsActive, true));

                if (!oldIsActive)
                    return pool;
            }

            memory_pool_t* newHead = new memory_pool_t;
            memory_pool_t* oldHead;
            do
            {
                oldHead = head.load();
            } while (head.compare_exchange_weak(oldHead, newHead));

            return newHead;
        }

    private:
        std::atomic<memory_pool_t*> head = nullptr;
    };

    memory_manager_t& getMemoryManager()
    {
        static memory_manager_t manager;

        return manager;
    }

    memory_pool_t* getMemoryPool()
    {
        static thread_local memory_pool_t* pool = nullptr;

        if (pool == nullptr || !pool->isActive.load())
            pool = getMemoryManager().obtain();

        return pool;
    }
}
