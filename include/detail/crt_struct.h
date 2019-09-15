#pragma once

#include <atomic>

#include "util.h"

namespace task_executor
{
    template<class T>
    struct lock_free_list
    {
        void pushBack(T data)
        {

        }

        void pushFront(T data)
        {

        }

        T popBack()
        {

        }

        T popFront()
        {
            
        }

        T& getBack()
        {
            return nullptr;
        }

        T& getFront()
        {
            return nullptr;
        }

        bool isEmpty()
        {
            return false;
        }
    };

    template<class T>
    struct lock_free_fixed_deque
    {
        void pushBack(T data)
        {

        }

        void pushFront(T data)
        {

        }

        T popBack()
        {

        }

        T popFront()
        {

        }

        T& getBack()
        {
            return nullptr;
        }

        T& getFront()
        {
            return nullptr;
        }

        bool isEmpty()
        {
            return false;
        }
    };

    template<class T>
    using crt_list = lock_free_list<T>;

    template<class T>
    using crt_fixed_deque = lock_free_fixed_deque<T>;

    template<class T>
    struct lock_free_list_deque
    {
        void pushBack(T data)
        {

        }

        void pushFront(T data)
        {

        }

        T popBack()
        {

        }

        T popFront()
        {

        }

        T& getBack()
        {
            return nullptr;
        }

        T& getFront()
        {
            return nullptr;
        }

        bool isEmpty()
        {
            return false;
        }
    };

    template<class T>
    using crt_list_deque = lock_free_list_deque<T>;

    template<class T>
    struct lock_free_set
    {

    };

    template<class T>
    using crt_set = lock_free_set<T>;

    template<class Key, class T>
    struct lock_free_map
    {

    };

    template<class Key, class T>
    using crt_map = lock_free_map<Key, T>;
}