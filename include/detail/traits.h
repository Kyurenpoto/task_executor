#pragma once

#include <type_traits>

namespace task_executor
{
    inline namespace traits_v1
    {
        template<class Executor, class = void>
        struct is_executor : std::false_type
        {};

        template<class Executor>
        constexpr bool is_executor_v =
            is_executor<Executor>::value;

        template<class Executable, class = void>
        struct is_executable : std::false_type
        {};

        template<class Executable>
        constexpr bool is_executable_v =
            is_executor<Executable>::value;

        template<class Awaiter, class = void>
        struct is_awaiter : std::false_type
        {};

        template<class Awaiter>
        constexpr bool is_awaiter_v =
            is_executor<Awaiter>::value;

        template<class Awaitable, class = void>
        struct is_awaitable : std::false_type
        {};

        template<class Awaitable>
        constexpr bool is_awaitable_v =
            is_executor<Awaitable>::value;

        template<class ExecutableContinuation, class = void>
        struct is_executable_continuation : std::false_type
        {};

        template<class ExecutableContinuation>
        constexpr bool is_executable_continuation_v =
            is_executable_continuation<ExecutableContinuation>::value;

        template<class AwaitableContinuation, class = void>
        struct is_awaitable_continuation : std::false_type
        {};

        template<class AwaitableContinuation>
        constexpr bool is_awaitable_continuation_v =
            is_awaitable_continuation<AwaitableContinuation>::value;

        template<class WrapedExecutableContinuation, class = void>
        struct is_wraped_executable_continuation : std::false_type
        {};

        template<class WrapedExecutableContinuation>
        constexpr bool is_wraped_executable_continuation_v =
            is_wraped_executable_continuation<WrapedExecutableContinuation>::value;

        template<class WrapedAwaitableContinuation, class = void>
        struct is_wraped_awaitable_continuation : std::false_type
        {};

        template<class WrapedAwaitableContinuation>
        constexpr bool is_wraped_awaitable_continuation_v =
            is_wraped_awaitable_continuation<WrapedAwaitableContinuation>::value;
    }
}
