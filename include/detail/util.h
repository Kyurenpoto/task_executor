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

    std::chrono::steady_clock::time_point getEpoch()
    {
        std::chrono::steady_clock::time_point now =
            std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration duration =
            now.time_since_epoch();

        return now - std::chrono::duration_cast<
            std::chrono::nanoseconds>(duration);
    }
}
