#pragma once

#include "../traits.h"

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            post(Executor &&, ExecutableContinuation&&)
        {
            static_assert(is_executor_v<Executor> &&
                is_executable_continuation_v<ExecutableContinuation>,
                "It doesn't satisfy executor traits or executable continuation traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, ExecutableContinuation>>>();
        }
        
        template<class Duration, class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            post_after(Executor &&, Duration &&, ExecutableContinuation&&)
        {
            static_assert(is_executor_v<Executor> &&
                is_executable_continuation_v<ExecutableContinuation>,
                "It doesn't satisfy executor traits or executable continuation traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, ExecutableContinuation>>>();
        }
        
        template<class TimePoint, class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            post_at(Executor &&, TimePoint &&, ExecutableContinuation&&)
        {
            static_assert(is_executor_v<Executor> &&
                is_executable_continuation_v<ExecutableContinuation>,
                "It doesn't satisfy executor traits or executable continuation traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, ExecutableContinuation>>>();
        }
        
        template<class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            dispatch(Executor &&, ExecutableContinuation&&)
        {
            static_assert(is_executor_v<Executor> &&
                is_executable_continuation_v<ExecutableContinuation>,
                "It doesn't satisfy executor traits or executable continuation traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, ExecutableContinuation>>>();
        }
        
        template<class Duration, class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            dispatch_after(Executor &&, Duration &&, ExecutableContinuation&&)
        {
            static_assert(is_executor_v<Executor> &&
                is_executable_continuation_v<ExecutableContinuation>,
                "It doesn't satisfy executor traits or executable continuation traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, ExecutableContinuation>>>();
        }
        
        template<class TimePoint, class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            dispatch_at(Executor &&, TimePoint &&, ExecutableContinuation&&)
        {
            static_assert(is_executor_v<Executor> &&
                is_executable_continuation_v<ExecutableContinuation>,
                "It doesn't satisfy executor traits or executable continuation traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, ExecutableContinuation>>>();
        }
        
        template<class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            defer(Executor &&, ExecutableContinuation&&)
        {
            static_assert(is_executor_v<Executor> &&
                is_executable_continuation_v<ExecutableContinuation>,
                "It doesn't satisfy executor traits or executable continuation traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, ExecutableContinuation>>>();
        }
        
        template<class Duration, class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            defer_after(Executor &&, Duration &&, ExecutableContinuation&&)
        {
            static_assert(is_executor_v<Executor> &&
                is_executable_continuation_v<ExecutableContinuation>,
                "It doesn't satisfy executor traits or executable continuation traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, ExecutableContinuation>>>();
        }
        
        template<class TimePoint, class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            defer_at(Executor &&, TimePoint &&, ExecutableContinuation&&)
        {
            static_assert(is_executor_v<Executor> &&
                is_executable_continuation_v<ExecutableContinuation>,
                "It doesn't satisfy executor traits or executable continuation traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, ExecutableContinuation>>>();
        }
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_post(Awaiter &&, AwaitableContinuation&&)
        {
            static_assert(is_awaiter_v<Awaiter> &&
                is_awaitable_continuation_v<AwaitableContinuation>,
                "It doesn't satisfy awaiter traits or awaitable continuation traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, AwaitableContinuation>>>();
        }
        
        template<class Duration, class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_post_after(Awaiter &&, Duration &&, AwaitableContinuation&&)
        {
            static_assert(is_awaiter_v<Awaiter> &&
                is_awaitable_continuation_v<AwaitableContinuation>,
                "It doesn't satisfy awaiter traits or awaitable continuation traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, AwaitableContinuation>>>();
        }
        
        template<class TimePoint, class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_post_at(Awaiter &&, TimePoint &&, AwaitableContinuation&&)
        {
            static_assert(is_awaiter_v<Awaiter> &&
                is_awaitable_continuation_v<AwaitableContinuation>,
                "It doesn't satisfy awaiter traits or awaitable continuation traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, AwaitableContinuation>>>();
        }
        
        template<class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_dispatch(Awaiter &&, AwaitableContinuation&&)
        {
            static_assert(is_awaiter_v<Awaiter> &&
                is_awaitable_continuation_v<AwaitableContinuation>,
                "It doesn't satisfy awaiter traits or awaitable continuation traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, AwaitableContinuation>>>();
        }
        
        template<class Duration, class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_dispatch_after(Awaiter &&, Duration &&, AwaitableContinuation&&)
        {
            static_assert(is_awaiter_v<Awaiter> &&
                is_awaitable_continuation_v<AwaitableContinuation>,
                "It doesn't satisfy awaiter traits or awaitable continuation traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, AwaitableContinuation>>>();
        }
        
        template<class TimePoint, class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_dispatch_at(Awaiter &&, TimePoint &&, AwaitableContinuation&&)
        {
            static_assert(is_awaiter_v<Awaiter> &&
                is_awaitable_continuation_v<AwaitableContinuation>,
                "It doesn't satisfy awaiter traits or awaitable continuation traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, AwaitableContinuation>>>();
        }
        
        template<class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_defer(Awaiter &&, AwaitableContinuation&&)
        {
            static_assert(is_awaiter_v<Awaiter> &&
                is_awaitable_continuation_v<AwaitableContinuation>,
                "It doesn't satisfy awaiter traits or awaitable continuation traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, AwaitableContinuation>>>();
        }
        
        template<class Duration, class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_defer_after(Awaiter &&, Duration &&, AwaitableContinuation&&)
        {
            static_assert(is_awaiter_v<Awaiter> &&
                is_awaitable_continuation_v<AwaitableContinuation>,
                "It doesn't satisfy awaiter traits or awaitable continuation traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, AwaitableContinuation>>>();
        }
        
        template<class TimePoint, class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_defer_at(Awaiter &&, TimePoint &&, AwaitableContinuation&&)
        {
            static_assert(is_awaiter_v<Awaiter> &&
                is_awaitable_continuation_v<AwaitableContinuation>,
                "It doesn't satisfy awaiter traits or awaitable continuation traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, AwaitableContinuation>>>();
        }
    }
}
