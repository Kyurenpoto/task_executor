#pragma once

#include "../traits.h"

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            post(WrapedExecutableContinuation && ...)
        {
            static_assert(is_wraped_executable_continuation_v<WrapedExecutableContinuation>...,
                "It doesn't satisfy wraped executable continuation traits");
        
            return std::declval<execution_result_t<WrapedExecutableContinuation...>>();
        }
        
        template<class Duration, class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            post_after(Duration &&, WrapedExecutableContinuation && ...)
        {
            static_assert(is_wraped_executable_continuation_v<WrapedExecutableContinuation>...,
                "It doesn't satisfy wraped executable continuation traits");
        
            return std::declval<execution_result_t<WrapedExecutableContinuation...>>();
        }
        
        template<class TimePoint, class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            post_at(TimePoint &&, WrapedExecutableContinuation && ...)
        {
            static_assert(is_wraped_executable_continuation_v<WrapedExecutableContinuation>...,
                "It doesn't satisfy wraped executable continuation traits");
        
            return std::declval<execution_result_t<WrapedExecutableContinuation...>>();
        }
        
        template<class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            dispatch(WrapedExecutableContinuation && ...)
        {
            static_assert(is_wraped_executable_continuation_v<WrapedExecutableContinuation>...,
                "It doesn't satisfy wraped executable continuation traits");
        
            return std::declval<execution_result_t<WrapedExecutableContinuation...>>();
        }
        
        template<class Duration, class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            dispatch_after(Duration &&, WrapedExecutableContinuation && ...)
        {
            static_assert(is_wraped_executable_continuation_v<WrapedExecutableContinuation>...,
                "It doesn't satisfy wraped executable continuation traits");
        
            return std::declval<execution_result_t<WrapedExecutableContinuation...>>();
        }
        
        template<class TimePoint, class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            dispatch_at(TimePoint &&, WrapedExecutableContinuation && ...)
        {
            static_assert(is_wraped_executable_continuation_v<WrapedExecutableContinuation>...,
                "It doesn't satisfy wraped executable continuation traits");
        
            return std::declval<execution_result_t<WrapedExecutableContinuation...>>();
        }
        
        template<class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            defer(WrapedExecutableContinuation && ...)
        {
            static_assert(is_wraped_executable_continuation_v<WrapedExecutableContinuation>...,
                "It doesn't satisfy wraped executable continuation traits");
        
            return std::declval<execution_result_t<WrapedExecutableContinuation...>>();
        }
        
        template<class Duration, class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            defer_after(Duration &&, WrapedExecutableContinuation && ...)
        {
            static_assert(is_wraped_executable_continuation_v<WrapedExecutableContinuation>...,
                "It doesn't satisfy wraped executable continuation traits");
        
            return std::declval<execution_result_t<WrapedExecutableContinuation...>>();
        }
        
        template<class TimePoint, class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            defer_at(TimePoint &&, WrapedExecutableContinuation && ...)
        {
            static_assert(is_wraped_executable_continuation_v<WrapedExecutableContinuation>...,
                "It doesn't satisfy wraped executable continuation traits");
        
            return std::declval<execution_result_t<WrapedExecutableContinuation...>>();
        }
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_post(WrapedAwaitableContinuation && ...)
        {
            static_assert(is_wraped_awaitable_continuation_v<WrapedAwaitableContinuation>...,
                "It doesn't satisfy wraped awaitable continuation traits");
        
            return std::declval<await_result_t<WrapedAwaitableContinuation...>>();
        }
        
        template<class Duration, class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_post_after(Duration &&, WrapedAwaitableContinuation && ...)
        {
            static_assert(is_wraped_awaitable_continuation_v<WrapedAwaitableContinuation>...,
                "It doesn't satisfy wraped awaitable continuation traits");
        
            return std::declval<await_result_t<WrapedAwaitableContinuation...>>();
        }
        
        template<class TimePoint, class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_post_at(TimePoint &&, WrapedAwaitableContinuation && ...)
        {
            static_assert(is_wraped_awaitable_continuation_v<WrapedAwaitableContinuation>...,
                "It doesn't satisfy wraped awaitable continuation traits");
        
            return std::declval<await_result_t<WrapedAwaitableContinuation...>>();
        }
        
        template<class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_dispatch(WrapedAwaitableContinuation && ...)
        {
            static_assert(is_wraped_awaitable_continuation_v<WrapedAwaitableContinuation>...,
                "It doesn't satisfy wraped awaitable continuation traits");
        
            return std::declval<await_result_t<WrapedAwaitableContinuation...>>();
        }
        
        template<class Duration, class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_dispatch_after(Duration &&, WrapedAwaitableContinuation && ...)
        {
            static_assert(is_wraped_awaitable_continuation_v<WrapedAwaitableContinuation>...,
                "It doesn't satisfy wraped awaitable continuation traits");
        
            return std::declval<await_result_t<WrapedAwaitableContinuation...>>();
        }
        
        template<class TimePoint, class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_dispatch_at(TimePoint &&, WrapedAwaitableContinuation && ...)
        {
            static_assert(is_wraped_awaitable_continuation_v<WrapedAwaitableContinuation>...,
                "It doesn't satisfy wraped awaitable continuation traits");
        
            return std::declval<await_result_t<WrapedAwaitableContinuation...>>();
        }
        
        template<class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_defer(WrapedAwaitableContinuation && ...)
        {
            static_assert(is_wraped_awaitable_continuation_v<WrapedAwaitableContinuation>...,
                "It doesn't satisfy wraped awaitable continuation traits");
        
            return std::declval<await_result_t<WrapedAwaitableContinuation...>>();
        }
        
        template<class Duration, class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_defer_after(Duration &&, WrapedAwaitableContinuation && ...)
        {
            static_assert(is_wraped_awaitable_continuation_v<WrapedAwaitableContinuation>...,
                "It doesn't satisfy wraped awaitable continuation traits");
        
            return std::declval<await_result_t<WrapedAwaitableContinuation...>>();
        }
        
        template<class TimePoint, class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_defer_at(TimePoint &&, WrapedAwaitableContinuation && ...)
        {
            static_assert(is_wraped_awaitable_continuation_v<WrapedAwaitableContinuation>...,
                "It doesn't satisfy wraped awaitable continuation traits");
        
            return std::declval<await_result_t<WrapedAwaitableContinuation...>>();
        }
    }
}
