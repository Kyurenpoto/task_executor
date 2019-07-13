#pragma once

#include <chrono>
#include <type_traits>

namespace task_executor
{
    inline namespace trait_v1
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
        awaiter_result_t<Awaiter, Awaitable...> post(Awaiter &&, Awaitable && ...);

        template<class Rep, class Period, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> post(Awaiter &&, const std::chrono::duration<Rep, Period> &, Awaitable && ...);

        template<class Clock, class Duration, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> post(Awaiter &&, const std::chrono::time_point<Clock, Duration> &, Awaitable && ...);

        template<class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> dispatch(Awaiter &&, Awaitable && ...);

        template<class Rep, class Period, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> dispatch(Awaiter &&, const std::chrono::duration<Rep, Period> &, Awaitable && ...);

        template<class Clock, class Duration, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> dispatch(Awaiter &&, const std::chrono::time_point<Clock, Duration> &, Awaitable && ...);

        template<class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> defer(Awaiter &&, Awaitable && ...);

        template<class Rep, class Period, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> defer(Awaiter &&, const std::chrono::duration<Rep, Period> &, Awaitable && ...);

        template<class Clock, class Duration, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> defer(Awaiter &&, const std::chrono::time_point<Clock, Duration> &, Awaitable && ...);
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
