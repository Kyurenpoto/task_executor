#pragma once

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
}