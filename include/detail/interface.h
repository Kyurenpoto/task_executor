#pragma once

#include <chrono>

#include "traits.h"

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class...>
        struct executor_result_t;

        template<class Executor, class... Executable>
        executor_result_t<Executor, Executable...> post(Executor &&, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "Cannot post: It doesn't satisfy executor traits or executable traits");

            return std::declval<executor_result_t<Executor, Executable>>();
        }

        template<class Rep, class Period, class Executor, class... Executable>
        executor_result_t<Executor, Executable...> post(Executor &&, const std::chrono::duration<Rep, Period> &, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "Cannot post: It doesn't satisfy executor traits or executable traits");

            return std::declval<executor_result_t<Executor, Executable>>();
        }

        template<class Clock, class Duration, class Executor, class... Executable>
        executor_result_t<Executor, Executable...> post(Executor &&, const std::chrono::time_point<Clock, Duration> &, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "Cannot post: It doesn't satisfy executor traits or executable traits");

            return std::declval<executor_result_t<Executor, Executable>>();
        }

        template<class Executor, class... Executable>
        executor_result_t<Executor, Executable...> dispatch(Executor &&, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "Cannot dispatch: It doesn't satisfy executor traits or executable traits");

            return std::declval<executor_result_t<Executor, Executable>>();
        }

        template<class Rep, class Period, class Executor, class... Executable>
        executor_result_t<Executor, Executable...> dispatch(Executor &&, const std::chrono::duration<Rep, Period> &, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "Cannot dispatch: It doesn't satisfy executor traits or executable traits");

            return std::declval<executor_result_t<Executor, Executable>>();
        }

        template<class Clock, class Duration, class Executor, class... Executable>
        executor_result_t<Executor, Executable...> dispatch(Executor &&, const std::chrono::time_point<Clock, Duration> &, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "Cannot dispatch: It doesn't satisfy executor traits or executable traits");

            return std::declval<executor_result_t<Executor, Executable>>();
        }

        template<class Executor, class... Executable>
        executor_result_t<Executor, Executable...> defer(Executor &&, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "Cannot defer: It doesn't satisfy executor traits or executable traits");

            return std::declval<executor_result_t<Executor, Executable>>();
        }

        template<class Rep, class Period, class Executor, class... Executable>
        executor_result_t<Executor, Executable...> defer(Executor &&, const std::chrono::duration<Rep, Period> &, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "Cannot defer: It doesn't satisfy executor traits or executable traits");

            return std::declval<executor_result_t<Executor, Executable>>();
        }

        template<class Clock, class Duration, class Executor, class... Executable>
        executor_result_t<Executor, Executable...> defer(Executor &&, const std::chrono::time_point<Clock, Duration> &, Executable && ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "Cannot defer: It doesn't satisfy executor traits or executable traits");

            return std::declval<executor_result_t<Executor, Executable>>();
        }



        template<class...>
        struct awaiter_result_t;

        template<class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_post(Awaiter &&, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "Cannot co_post: It doesn't satisfy awaiter traits or awaitable traits");

            return std::declval<awaiter_result_t<Awaiter, Awaitable>>();
        }

        template<class Rep, class Period, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_post(Awaiter &&, const std::chrono::duration<Rep, Period> &, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "Cannot co_post: It doesn't satisfy awaiter traits or awaitable traits");

            return std::declval<awaiter_result_t<Awaiter, Awaitable>>();
        }

        template<class Clock, class Duration, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_post(Awaiter &&, const std::chrono::time_point<Clock, Duration> &, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "Cannot co_post: It doesn't satisfy awaiter traits or awaitable traits");

            return std::declval<awaiter_result_t<Awaiter, Awaitable>>();
        }

        template<class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_dispatch(Awaiter &&, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "Cannot co_dispatch: It doesn't satisfy awaiter traits or awaitable traits");

            return std::declval<awaiter_result_t<Awaiter, Awaitable>>();
        }

        template<class Rep, class Period, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_dispatch(Awaiter &&, const std::chrono::duration<Rep, Period> &, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "Cannot co_dispatch: It doesn't satisfy awaiter traits or awaitable traits");

            return std::declval<awaiter_result_t<Awaiter, Awaitable>>();
        }

        template<class Clock, class Duration, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_dispatch(Awaiter &&, const std::chrono::time_point<Clock, Duration> &, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "Cannot co_dispatch: It doesn't satisfy awaiter traits or awaitable traits");

            return std::declval<awaiter_result_t<Awaiter, Awaitable>>();
        }

        template<class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_defer(Awaiter &&, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "Cannot co_defer: It doesn't satisfy awaiter traits or awaitable traits");

            return std::declval<awaiter_result_t<Awaiter, Awaitable>>();
        }

        template<class Rep, class Period, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_defer(Awaiter &&, const std::chrono::duration<Rep, Period> &, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "Cannot co_defer: It doesn't satisfy awaiter traits or awaitable traits");

            return std::declval<awaiter_result_t<Awaiter, Awaitable>>();
        }

        template<class Clock, class Duration, class Awaiter, class... Awaitable>
        awaiter_result_t<Awaiter, Awaitable...> co_defer(Awaiter &&, const std::chrono::time_point<Clock, Duration> &, Awaitable && ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "Cannot co_defer: It doesn't satisfy awaiter traits or awaitable traits");

            return std::declval<awaiter_result_t<Awaiter, Awaitable>>();
        }
    }
}
