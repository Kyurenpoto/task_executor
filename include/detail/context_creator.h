#pragma once

#include <utility>

namespace task_executor
{
    template<class T>
    struct context_creator_t
    {
        template<class Func, class... Args>
        context_creator_t* post(Func&& func, Args&& ... args)
        {
            return getConcrete<T&>(*this).post(std::forward(func), std::forward(args)...);
        }

        template<class Obj, class Func, class... Args>
        context_creator_t* post(Obj* obj, Func&& func, Args&& ... args)
        {
            return getConcrete<T&>(*this).post(obj, std::forward(func), std::forward(args)...);
        }
    };
}