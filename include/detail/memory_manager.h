#pragma once

#include <atomic>
#include <memory>
#include <memory_resource>

namespace task_executor
{
    struct memory_pool_t
    {
        memory_pool_t* next = nullptr;
        std::atomic_bool isActive = true;

        void leaveOwner()
        {
            isActive.store(false);
        }

        template<class T, class... Args>
        T* xnew(Args&&... args)
        {
            T* ptr = static_cast<T*>(resource.allocate(sizeof(T), alignof(T)));

            return new(ptr) T{ std::forward<Args>(args)... };
        }

        template<class T>
        T* xnew(T&& arg)
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
            memory_pool_t* old = obtainOld();
            if (old != nullptr)
                return old;

            memory_pool_t* newHead = new memory_pool_t;
            memory_pool_t* oldHead;
            do
            {
                oldHead = head.load();
                newHead->next = oldHead;
            } while (!head.compare_exchange_weak(oldHead, newHead));

            return newHead;
        }

    private:
        std::atomic<memory_pool_t*> head = nullptr;

        memory_pool_t* obtainOld()
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

            return nullptr;
        }
    };

    template<class T>
    struct xmanaged_ptr
    {
        xmanaged_ptr() = default;
        xmanaged_ptr(const xmanaged_ptr&) = delete;
        xmanaged_ptr& operator=(const xmanaged_ptr&) = delete;

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

        template<class U>
        xmanaged_ptr<T>& operator=(xmanaged_ptr<U>&& xmptr) noexcept
        {
            static_assert(std::is_convertible_v<T*, U*> || std::is_convertible_v<U*, T*>);

            std::swap(ptr, xmptr.ptr);
            std::swap(deleter, xmptr.deleter);

            return *this;
        }

        ~xmanaged_ptr()
        {
            if (deleter)
                deleter(ptr);
        }

        T* operator->() const noexcept
        {
            return static_cast<T*>(ptr);
        }

        std::add_lvalue_reference_t<T> operator*() const
        {
            return *static_cast<T*>(ptr);
        }

    private:
        void* ptr;
        std::function<void(void*)> deleter;

        template<class U>
        friend struct xmanaged_ptr;
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
