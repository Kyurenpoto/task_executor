#pragma once

#include <type_traits>
#include <chrono>

namespace task_executor
{
    template<class T, class U>
    T& getConcrete(U& obj)
    {
        return static_cast<T&>(const_cast<U&>(obj));
    }

    template<class T, class U>
    T* getConcrete(U* obj)
    {
        return static_cast<T*>(const_cast<U*>(obj));
    }

    template<class A, class B>
    bool isSameObj(A* a, B* b)
    {
        if constexpr (std::is_convertible_v<A, B>)
            return a == static_cast<B*>(a);
        else if constexpr (std::is_convertible_v<B, A>)
            return a == static_cast<A*>(b);
        else
            return false;
    }

    using time_point_t = std::chrono::steady_clock::time_point;
    using duration_t = std::chrono::steady_clock::duration;

    time_point_t getEpoch()
    {
        time_point_t now = std::chrono::steady_clock::now();
        duration_t duration = now.time_since_epoch();

        return now - std::chrono::duration_cast<
            std::chrono::nanoseconds>(duration);
    }
}
