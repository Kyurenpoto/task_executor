#pragma once

#include "../traits.h"

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            post(Executor &&, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, executable_continuation_t<Executable...>>>>();
        }
        
        template<class Duration, class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            post_after(Executor &&, Duration &&, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, executable_continuation_t<Executable...>>>>();
        }
        
        template<class TimePoint, class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            post_at(Executor &&, TimePoint &&, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, executable_continuation_t<Executable...>>>>();
        }
        
        template<class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            dispatch(Executor &&, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, executable_continuation_t<Executable...>>>>();
        }
        
        template<class Duration, class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            dispatch_after(Executor &&, Duration &&, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, executable_continuation_t<Executable...>>>>();
        }
        
        template<class TimePoint, class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            dispatch_at(Executor &&, TimePoint &&, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, executable_continuation_t<Executable...>>>>();
        }
        
        template<class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            defer(Executor &&, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, executable_continuation_t<Executable...>>>>();
        }
        
        template<class Duration, class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            defer_after(Executor &&, Duration &&, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, executable_continuation_t<Executable...>>>>();
        }
        
        template<class TimePoint, class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            defer_at(Executor &&, TimePoint &&, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");
        
            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, executable_continuation_t<Executable...>>>>();
        }
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_post(Awaiter &&, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, awaitable_continuation_t<Awaitable...>>>>();
        }
        
        template<class Duration, class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_post_after(Awaiter &&, Duration &&, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, awaitable_continuation_t<Awaitable...>>>>();
        }
        
        template<class TimePoint, class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_post_at(Awaiter &&, TimePoint &&, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, awaitable_continuation_t<Awaitable...>>>>();
        }
        
        template<class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_dispatch(Awaiter &&, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, awaitable_continuation_t<Awaitable...>>>>();
        }
        
        template<class Duration, class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_dispatch_after(Awaiter &&, Duration &&, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, awaitable_continuation_t<Awaitable...>>>>();
        }
        
        template<class TimePoint, class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_dispatch_at(Awaiter &&, TimePoint &&, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, awaitable_continuation_t<Awaitable...>>>>();
        }
        
        template<class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_defer(Awaiter &&, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, awaitable_continuation_t<Awaitable...>>>>();
        }
        
        template<class Duration, class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_defer_after(Awaiter &&, Duration &&, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, awaitable_continuation_t<Awaitable...>>>>();
        }
        
        template<class TimePoint, class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_defer_at(Awaiter &&, TimePoint &&, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");
        
            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, awaitable_continuation_t<Awaitable...>>>>();
        }
    }
}
