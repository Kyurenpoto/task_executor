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
        T* xnew(Args&&... args)
        {
            T* ptr = static_cast<T*>(resource.allocate(sizeof(T), alignof(T)));

            return new(ptr) T{ std::forward<Args>(args)... };
        }

        template<class T>
        T* xnew(T arg)
        {
            T* ptr = static_cast<T*>(resource.allocate(sizeof(T), alignof(T)));

            return new(ptr) T{ std::forward<T>(arg) };
        }

        template<class T>
        void xdelete(T* ptr)
        {
            ptr->~T();

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
                    !pool->isActive.compare_exchange_weak(oldIsActive, true));

                if (!oldIsActive)
                    return pool;
            }

            memory_pool_t* newHead = new memory_pool_t;
            memory_pool_t* oldHead;
            do
            {
                oldHead = head.load();
            } while (!head.compare_exchange_weak(oldHead, newHead));

            return newHead;
        }

    private:
        std::atomic<memory_pool_t*> head = nullptr;
    };

    template<class T>
    struct xmanaged_ptr
    {
        template<class Func>
        xmanaged_ptr(T* p, Func&& d) noexcept :
            ptr{ p },
            deleter{ std::function<void(void*)>{ d } }
        {}

        template<class U>
        xmanaged_ptr(xmanaged_ptr<U>&& xmptr) noexcept :
            ptr{ xmptr.ptr },
            deleter{ xmptr.deleter }
        {
            xmptr.ptr = nullptr;
            xmptr.deleter = {};
        }

        ~xmanaged_ptr()
        {
            if (deleter)
                deleter(ptr);
        }

        T* operator->() const noexcept
        {
            return ptr;
        }

        std::add_lvalue_reference_t<T> operator*() const
        {
            return *ptr;
        }

    private:
        T* ptr;
        std::function<void(void*)> deleter;
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

    template<class T, class... Args>
    xmanaged_ptr<T> make_xmanaged(Args&&... args)
    {
        return xmanaged_ptr{ getMemoryPool()->xnew<T>(
            std::forward<Args>(args)...),
            [pool = getMemoryPool()](void* ptr) {
            pool->xdelete(static_cast<T*>(ptr)); } };
    }

    template<class T>
    xmanaged_ptr<T> make_xmanaged(T arg)
    {
        return xmanaged_ptr{ getMemoryPool()->xnew<T>(
            std::forward<T>(arg)),
            [pool = getMemoryPool()](void* ptr) {
            pool->xdelete(static_cast<T*>(ptr)); } };
    }
}
