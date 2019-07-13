#pragma once

namespace task_executor
{
    inline namespace traits_v1
    {
        template<class Executor, class = void>
        struct is_executor : std::false_type
        {};

        template<class Executable, class = void>
        struct is_executable : std::false_type
        {};

        template<class Awaiter, class = void>
        struct is_awaiter : std::false_type
        {};

        template<class Awaitable, class = void>
        struct is_awaitable : std::false_type
        {};
    }
}
