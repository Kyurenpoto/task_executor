#pragma once

namespace task_executor
{
    template<class...>
    struct execution_result;

    template<class... T>
    using execution_result_t =
        typename execution_result<T...>::type;

    template<class...>
    struct await_result;

    template<class... T>
    using await_result_t =
        typename await_result<T...>::type;

    template<class, class>
    struct executable_continuation_wrap;

    template<class T, class U>
    using executable_continuation_wrap_t =
        typename executable_continuation_wrap<T, U>::type;
    
    template<class, class>
    struct awaitable_continuation_wrap;

    template<class T, class U>
    using awaitable_continuation_wrap_t =
        typename awaitable_continuation_wrap<T, U>::type;

    template<class...>
    struct executable_continuation;

    template<class... T>
    using executable_continuation_t =
        typename executable_continuation<T...>::type;

    template<class...>
    struct awaitable_continuation;

    template<class... T>
    using awaitable_continuation_t =
        typename awaitable_continuation<T...>::type;
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            post(Executor &&, Executable &&...);
    
        template<class Duration, class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            post_after(Executor &&, Duration &&, Executable &&...);
    
        template<class TimePoint, class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            post_at(Executor &&, TimePoint &&, Executable &&...);
    
        template<class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            dispatch(Executor &&, Executable &&...);
    
        template<class Duration, class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            dispatch_after(Executor &&, Duration &&, Executable &&...);
    
        template<class TimePoint, class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            dispatch_at(Executor &&, TimePoint &&, Executable &&...);
    
        template<class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            defer(Executor &&, Executable &&...);
    
        template<class Duration, class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            defer_after(Executor &&, Duration &&, Executable &&...);
    
        template<class TimePoint, class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            defer_at(Executor &&, TimePoint &&, Executable &&...);
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_post(Awaiter &&, Awaitable &&...);
    
        template<class Duration, class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_post_after(Awaiter &&, Duration &&, Awaitable &&...);
    
        template<class TimePoint, class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_post_at(Awaiter &&, TimePoint &&, Awaitable &&...);
    
        template<class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_dispatch(Awaiter &&, Awaitable &&...);
    
        template<class Duration, class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_dispatch_after(Awaiter &&, Duration &&, Awaitable &&...);
    
        template<class TimePoint, class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_dispatch_at(Awaiter &&, TimePoint &&, Awaitable &&...);
    
        template<class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_defer(Awaiter &&, Awaitable &&...);
    
        template<class Duration, class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_defer_after(Awaiter &&, Duration &&, Awaitable &&...);
    
        template<class TimePoint, class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_defer_at(Awaiter &&, TimePoint &&, Awaitable &&...);
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            post(Executor &&, ExecutableContinuation &&);
    
        template<class Duration, class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            post_after(Executor &&, Duration &&, ExecutableContinuation &&);
    
        template<class TimePoint, class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            post_at(Executor &&, TimePoint &&, ExecutableContinuation &&);
    
        template<class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            dispatch(Executor &&, ExecutableContinuation &&);
    
        template<class Duration, class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            dispatch_after(Executor &&, Duration &&, ExecutableContinuation &&);
    
        template<class TimePoint, class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            dispatch_at(Executor &&, TimePoint &&, ExecutableContinuation &&);
    
        template<class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            defer(Executor &&, ExecutableContinuation &&);
    
        template<class Duration, class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            defer_after(Executor &&, Duration &&, ExecutableContinuation &&);
    
        template<class TimePoint, class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            defer_at(Executor &&, TimePoint &&, ExecutableContinuation &&);
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_post(Awaiter &&, AwaitableContinuation &&);
    
        template<class Duration, class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_post_after(Awaiter &&, Duration &&, AwaitableContinuation &&);
    
        template<class TimePoint, class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_post_at(Awaiter &&, TimePoint &&, AwaitableContinuation &&);
    
        template<class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_dispatch(Awaiter &&, AwaitableContinuation &&);
    
        template<class Duration, class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_dispatch_after(Awaiter &&, Duration &&, AwaitableContinuation &&);
    
        template<class TimePoint, class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_dispatch_at(Awaiter &&, TimePoint &&, AwaitableContinuation &&);
    
        template<class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_defer(Awaiter &&, AwaitableContinuation &&);
    
        template<class Duration, class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_defer_after(Awaiter &&, Duration &&, AwaitableContinuation &&);
    
        template<class TimePoint, class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_defer_at(Awaiter &&, TimePoint &&, AwaitableContinuation &&);
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            post(WrapedExecutableContinuation &&...);
    
        template<class Duration, class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            post_after(Duration &&, WrapedExecutableContinuation &&...);
    
        template<class TimePoint, class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            post_at(TimePoint &&, WrapedExecutableContinuation &&...);
    
        template<class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            dispatch(WrapedExecutableContinuation &&...);
    
        template<class Duration, class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            dispatch_after(Duration &&, WrapedExecutableContinuation &&...);
    
        template<class TimePoint, class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            dispatch_at(TimePoint &&, WrapedExecutableContinuation &&...);
    
        template<class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            defer(WrapedExecutableContinuation &&...);
    
        template<class Duration, class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            defer_after(Duration &&, WrapedExecutableContinuation &&...);
    
        template<class TimePoint, class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            defer_at(TimePoint &&, WrapedExecutableContinuation &&...);
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_post(WrapedAwaitableContinuation &&...);
    
        template<class Duration, class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_post_after(Duration &&, WrapedAwaitableContinuation &&...);
    
        template<class TimePoint, class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_post_at(TimePoint &&, WrapedAwaitableContinuation &&...);
    
        template<class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_dispatch(WrapedAwaitableContinuation &&...);
    
        template<class Duration, class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_dispatch_after(Duration &&, WrapedAwaitableContinuation &&...);
    
        template<class TimePoint, class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_dispatch_at(TimePoint &&, WrapedAwaitableContinuation &&...);
    
        template<class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_defer(WrapedAwaitableContinuation &&...);
    
        template<class Duration, class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_defer_after(Duration &&, WrapedAwaitableContinuation &&...);
    
        template<class TimePoint, class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_defer_at(TimePoint &&, WrapedAwaitableContinuation &&...);
    }
}

namespace task_executor
{
    inline namespace executor_v1
    {
        struct executor;

        template<class Owner>
        struct local_executor;

        template<class Owner>
        struct global_executor;

        struct executor_distributor;
    }
}

namespace task_executor
{
    inline namespace awaiter_v1
    {
        struct awaiter;
    }
}

namespace task_executor
{
    inline namespace controller_v1
    {
        struct system;

        struct thread_pool;

        struct strand;
    }
}

#include "detail/traits.h"
#include "detail/interface.h"
