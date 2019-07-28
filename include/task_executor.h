#pragma once

namespace
{
    enum class op_policy
    {
        seq,
        par,
        unseq,
        par_unseq
    };
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Executor, class... Executable>
        auto post(op_policy, Executor && executor, Executable && ... executable);

        template<class Executor, class... Executable>
        auto dispatch(op_policy, Executor && executor, Executable && ... executable);

        template<class Executor, class... Executable>
        auto defer(op_policy, Executor && executor, Executable && ... executable);

        template<class Executor, class Duration, class... Executable>
        auto post_after(op_policy, Executor && executor, Duration && duration, Executable && ... executable);

        template<class Executor, class Duration, class... Executable>
        auto dispatch_after(op_policy, Executor && executor, Duration && duration, Executable && ... executable);

        template<class Executor, class Duration, class... Executable>
        auto defer_after(op_policy, Executor && executor, Duration && duration, Executable && ... executable);

        template<class Executor, class TimePoint, class... Executable>
        auto post_at(op_policy, Executor && executor, TimePoint && timepoint, Executable && ... executable);

        template<class Executor, class TimePoint, class... Executable>
        auto dispatch_at(op_policy, Executor && executor, TimePoint && timepoint, Executable && ... executable);

        template<class Executor, class TimePoint, class... Executable>
        auto defer_at(op_policy, Executor && executor, TimePoint && timepoint, Executable && ... executable);
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Awaiter, class... Awaitable>
        auto co_post(op_policy, Awaiter && awaiter, Awaitable && ... awaitable);

        template<class Awaiter, class... Awaitable>
        auto co_dispatch(op_policy, Awaiter && awaiter, Awaitable && ... awaitable);

        template<class Awaiter, class... Awaitable>
        auto co_defer(op_policy, Awaiter && awaiter, Awaitable && ... awaitable);

        template<class Awaiter, class Duration, class... Awaitable>
        auto co_post_after(op_policy, Awaiter && awaiter, Duration && duration, Awaitable && ... awaitable);

        template<class Awaiter, class Duration, class... Awaitable>
        auto co_dispatch_after(op_policy, Awaiter && awaiter, Duration && duration, Awaitable && ... awaitable);

        template<class Awaiter, class Duration, class... Awaitable>
        auto co_defer_after(op_policy, Awaiter && awaiter, Duration && duration, Awaitable && ... awaitable);

        template<class Awaiter, class TimePoint, class... Awaitable>
        auto co_post_at(op_policy, Awaiter && awaiter, TimePoint && timepoint, Awaitable && ... awaitable);

        template<class Awaiter, class TimePoint, class... Awaitable>
        auto co_dispatch_at(op_policy, Awaiter && awaiter, TimePoint && timepoint, Awaitable && ... awaitable);

        template<class Awaiter, class TimePoint, class... Awaitable>
        auto co_defer_at(op_policy, Awaiter && awaiter, TimePoint && timepoint, Awaitable && ... awaitable);
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

#include "detail/graph.h"
#include "detail/traits.h"
#include "detail/interface.h"
#include "detail/controller.h"
