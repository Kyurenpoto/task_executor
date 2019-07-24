#pragma once

#include "detail/traits.h"

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Executor, class... Executable>
        auto post(op_policy policy, Executor && executor, Executable && ... executable)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");

            return executor.post(executable...);
        }

        template<class Executor, class... Executable>
        auto dispatch(op_policy policy, Executor && executor, Executable && ... executable)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");

            return executor.dispatch(executable...);
        }

        template<class Executor, class... Executable>
        auto defer(op_policy policy, Executor && executor, Executable && ... executable)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");

            return executor.defer(executable...);
        }

        template<class Executor, class Duration, class... Executable>
        auto post_after(op_policy policy, Executor && executor, Duration && duration, Executable && ... executable)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");

            return executor.post_after(duration, executable...);
        }

        template<class Executor, class Duration, class... Executable>
        auto dispatch_after(op_policy policy, Executor && executor, Duration && duration, Executable && ... executable)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");

            return executor.dispatch_after(duration, executable...);
        }

        template<class Executor, class Duration, class... Executable>
        auto defer_after(op_policy policy, Executor && executor, Duration && duration, Executable && ... executable)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");

            return executor.defer_after(duration, executable...);
        }

        template<class Executor, class TimePoint, class... Executable>
        auto post_at(op_policy policy, Executor && executor, TimePoint && timepoint, Executable && ... executable)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");

            return executor.post_at(timepoint, executable...);
        }

        template<class Executor, class TimePoint, class... Executable>
        auto dispatch_at(op_policy policy, Executor && executor, TimePoint && timepoint, Executable && ... executable)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");

            return executor.dispatch_at(timepoint, executable...);
        }

        template<class Executor, class TimePoint, class... Executable>
        auto defer_at(op_policy policy, Executor && executor, TimePoint && timepoint, Executable && ... executable)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");

            return executor.defer_at(timepoint, executable...);
        }
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Awaiter, class... Awaitable>
        auto co_post(op_policy policy, Awaiter && awaiter, Awaitable && ... awaitable)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");

            return awaiter.co_post(awaitable...);
        }

        template<class Awaiter, class... Awaitable>
        auto co_dispatch(op_policy policy, Awaiter && awaiter, Awaitable && ... awaitable)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");

            return awaiter.co_dispatch(awaitable...);
        }

        template<class Awaiter, class... Awaitable>
        auto co_defer(op_policy policy, Awaiter && awaiter, Awaitable && ... awaitable)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");

            return awaiter.co_defer(awaitable...);
        }

        template<class Awaiter, class Duration, class... Awaitable>
        auto co_post_after(op_policy policy, Awaiter && awaiter, Duration && duration, Awaitable && ... awaitable)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");

            return awaiter.co_post_after(duration, awaitable...);
        }

        template<class Awaiter, class Duration, class... Awaitable>
        auto co_dispatch_after(op_policy policy, Awaiter && awaiter, Duration && duration, Awaitable && ... awaitable)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");

            return awaiter.co_dispatch_after(duration, awaitable...);
        }

        template<class Awaiter, class Duration, class... Awaitable>
        auto co_defer_after(op_policy policy, Awaiter && awaiter, Duration && duration, Awaitable && ... awaitable)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");

            return awaiter.co_defer_after(duration, awaitable...);
        }

        template<class Awaiter, class TimePoint, class... Awaitable>
        auto co_post_at(op_policy policy, Awaiter && awaiter, TimePoint && timepoint, Awaitable && ... awaitable)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");

            return awaiter.co_post_at(timepoint, awaitable...);
        }

        template<class Awaiter, class TimePoint, class... Awaitable>
        auto co_dispatch_at(op_policy policy, Awaiter && awaiter, TimePoint && timepoint, Awaitable && ... awaitable)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");

            return awaiter.co_dispatch_at(timepoint, awaitable...);
        }

        template<class Awaiter, class TimePoint, class... Awaitable>
        auto co_defer_at(op_policy policy, Awaiter && awaiter, TimePoint && timepoint, Awaitable && ... awaitable)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");

            return awaiter.co_defer_at(timepoint, awaitable...);
        }
    }
}
