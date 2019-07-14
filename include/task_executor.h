#pragma once

#include <chrono>

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class...>
        struct executor_result_t;

        template<class Executor, class... Executable>
        executor_result_t<Executor, Executable...> post(Executor &&, Executable && ...);

        template<class Rep, class Period, class Executor, class... Executable>
        executor_result_t<Executor, Executable...> post(Executor &&, const std::chrono::duration<Rep, Period> &, Executable && ...);

        template<class Clock, class Duration, class Executor, class... Executable>
        executor_result_t<Executor, Executable...> post(Executor &&, const std::chrono::time_point<Clock, Duration> &, Executable && ...);

        template<class Executor, class... Executable>
        executor_result_t<Executor, Executable...> dispatch(Executor &&, Executable && ...);

        template<class Rep, class Period, class Executor, class... Executable>
        executor_result_t<Executor, Executable...> dispatch(Executor &&, const std::chrono::duration<Rep, Period> &, Executable && ...);

        template<class Clock, class Duration, class Executor, class... Executable>
        executor_result_t<Executor, Executable...> dispatch(Executor &&, const std::chrono::time_point<Clock, Duration> &, Executable && ...);

        template<class Executor, class... Executable>
        executor_result_t<Executor, Executable...> defer(Executor &&, Executable && ...);

        template<class Rep, class Period, class Executor, class... Executable>
        executor_result_t<Executor, Executable...> defer(Executor &&, const std::chrono::duration<Rep, Period> &, Executable && ...);

        template<class Clock, class Duration, class Executor, class... Executable>
        executor_result_t<Executor, Executable...> defer(Executor &&, const std::chrono::time_point<Clock, Duration> &, Executable && ...);



        template<class...>
        struct awaiter_result_t;

        template<class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_post(Awaiter &&, Awaitable && ...);

        template<class Rep, class Period, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_post(Awaiter &&, const std::chrono::duration<Rep, Period> &, Awaitable && ...);

        template<class Clock, class Duration, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_post(Awaiter &&, const std::chrono::time_point<Clock, Duration> &, Awaitable && ...);

        template<class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_dispatch(Awaiter &&, Awaitable && ...);

        template<class Rep, class Period, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_dispatch(Awaiter &&, const std::chrono::duration<Rep, Period> &, Awaitable && ...);

        template<class Clock, class Duration, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_dispatch(Awaiter &&, const std::chrono::time_point<Clock, Duration> &, Awaitable && ...);

        template<class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_defer(Awaiter &&, Awaitable && ...);

        template<class Rep, class Period, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_defer(Awaiter &&, const std::chrono::duration<Rep, Period> &, Awaitable && ...);

        template<class Clock, class Duration, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_defer(Awaiter &&, const std::chrono::time_point<Clock, Duration> &, Awaitable && ...);
    }

    inline namespace executor_v1
    {
        struct executor;

        template<class Owner>
        struct local_executor;

        template<class Owner>
        struct global_executor;

        struct executor_distributor;
    }

    inline namespace awaiter_v1
    {
        struct awaiter;
    }

    inline namespace controller_v1
    {
        struct system;

        struct thread_pool;

        struct strand;
    }
}

#include "detail/traits.h"
#include "detail/interface.h"
