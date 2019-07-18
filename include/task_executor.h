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
        template<class... T>
        auto post(T && ... args);

        template<class... T>
        auto dispatch(T && ... args);

        template<class... T>
        auto defer(T && ... args);

        template<class... T>
        auto post_after(T && ... args);

        template<class... T>
        auto dispatch_after(T && ... args);

        template<class... T>
        auto defer_after(T && ... args);

        template<class... T>
        auto post_at(T && ... args);

        template<class... T>
        auto dispatch_at(T && ... args);

        template<class... T>
        auto defer_at(T && ... args);
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class... T>
        auto co_post(T && ... args);

        template<class... T>
        auto co_dispatch(T && ... args);

        template<class... T>
        auto co_defer(T && ... args);

        template<class... T>
        auto co_post_after(T && ... args);

        template<class... T>
        auto co_dispatch_after(T && ... args);

        template<class... T>
        auto co_defer_after(T && ... args);

        template<class... T>
        auto co_post_at(T && ... args);

        template<class... T>
        auto co_dispatch_at(T && ... args);

        template<class... T>
        auto co_defer_at(T && ... args);
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
