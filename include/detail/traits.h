#pragma once

namespace task_executor
{
    inline namespace traits_v1
    {
        template<class Executor, class = void>
        struct is_executor : std::false_type
        {};

        template<class Executor>
        constexpr bool is_executor_v = is_executor<Executor>::value;

        template<class Executable, class = void>
        struct is_executable : std::false_type
        {};

        template<class Executable>
        constexpr bool is_executable_v = is_executor<Executable>::value;

        template<class Awaiter, class = void>
        struct is_awaiter : std::false_type
        {};

        template<class Awaiter>
        constexpr bool is_awaiter_v = is_executor<Awaiter>::value;

        template<class Awaitable, class = void>
        struct is_awaitable : std::false_type
        {};

        template<class Awaitable>
        constexpr bool is_awaitable_v = is_executor<Awaitable>::value;
    }
}
