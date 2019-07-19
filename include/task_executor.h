#pragma once

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Executor, class... Executable>
        auto post(Executor && executor, Executable && ... executable);

        template<class Executor, class... Executable>
        auto dispatch(Executor && executor, Executable && ... executable);

        template<class Executor, class... Executable>
        auto defer(Executor && executor, Executable && ... executable);

        template<class Executor, class Duration, class... Executable>
        auto post_after(Executor && executor, Duration && duration, Executable && ... executable);

        template<class Executor, class Duration, class... Executable>
        auto dispatch_after(Executor && executor, Duration && duration, Executable && ... executable);

        template<class Executor, class Duration, class... Executable>
        auto defer_after(Executor && executor, Duration && duration, Executable && ... executable);

        template<class Executor, class TimePoint, class... Executable>
        auto post_at(Executor && executor, TimePoint && timepoint, Executable && ... executable);

        template<class Executor, class TimePoint, class... Executable>
        auto dispatch_at(Executor && executor, TimePoint && timepoint, Executable && ... executable);

        template<class Executor, class TimePoint, class... Executable>
        auto defer_at(Executor && executor, TimePoint && timepoint, Executable && ... executable);
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Awaiter, class... Awaitable>
        auto co_post(Awaiter && awaiter, Awaitable && ... awaitable);

        template<class Awaiter, class... Awaitable>
        auto co_dispatch(Awaiter && awaiter, Awaitable && ... awaitable);

        template<class Awaiter, class... Awaitable>
        auto co_defer(Awaiter && awaiter, Awaitable && ... awaitable);

        template<class Awaiter, class Duration, class... Awaitable>
        auto co_post_after(Awaiter && awaiter, Duration && duration, Awaitable && ... awaitable);

        template<class Awaiter, class Duration, class... Awaitable>
        auto co_dispatch_after(Awaiter && awaiter, Duration && duration, Awaitable && ... awaitable);

        template<class Awaiter, class Duration, class... Awaitable>
        auto co_defer_after(Awaiter && awaiter, Duration && duration, Awaitable && ... awaitable);

        template<class Awaiter, class TimePoint, class... Awaitable>
        auto co_post_at(Awaiter && awaiter, TimePoint && timepoint, Awaitable && ... awaitable);

        template<class Awaiter, class TimePoint, class... Awaitable>
        auto co_dispatch_at(Awaiter && awaiter, TimePoint && timepoint, Awaitable && ... awaitable);

        template<class Awaiter, class TimePoint, class... Awaitable>
        auto co_defer_at(Awaiter && awaiter, TimePoint && timepoint, Awaitable && ... awaitable);
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Executor, class... Executable>
        auto crt_post(Executor && executor, Executable && ... executable);

        template<class Executor, class... Executable>
        auto crt_dispatch(Executor && executor, Executable && ... executable);

        template<class Executor, class... Executable>
        auto crt_defer(Executor && executor, Executable && ... executable);

        template<class Executor, class Duration, class... Executable>
        auto crt_post_after(Executor && executor, Duration && duration, Executable && ... executable);

        template<class Executor, class Duration, class... Executable>
        auto crt_dispatch_after(Executor && executor, Duration && duration, Executable && ... executable);

        template<class Executor, class Duration, class... Executable>
        auto crt_defer_after(Executor && executor, Duration && duration, Executable && ... executable);

        template<class Executor, class TimePoint, class... Executable>
        auto crt_post_at(Executor && executor, TimePoint && timepoint, Executable && ... executable);

        template<class Executor, class TimePoint, class... Executable>
        auto crt_dispatch_at(Executor && executor, TimePoint && timepoint, Executable && ... executable);

        template<class Executor, class TimePoint, class... Executable>
        auto crt_defer_at(Executor && executor, TimePoint && timepoint, Executable && ... executable);
    }
}

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Awaiter, class... Awaitable>
        auto crt_co_post(Awaiter && awaiter, Awaitable && ... awaitable);

        template<class Awaiter, class... Awaitable>
        auto crt_co_dispatch(Awaiter && awaiter, Awaitable && ... awaitable);

        template<class Awaiter, class... Awaitable>
        auto crt_co_defer(Awaiter && awaiter, Awaitable && ... awaitable);

        template<class Awaiter, class Duration, class... Awaitable>
        auto crt_co_post_after(Awaiter && awaiter, Duration && duration, Awaitable && ... awaitable);

        template<class Awaiter, class Duration, class... Awaitable>
        auto crt_co_dispatch_after(Awaiter && awaiter, Duration && duration, Awaitable && ... awaitable);

        template<class Awaiter, class Duration, class... Awaitable>
        auto crt_co_defer_after(Awaiter && awaiter, Duration && duration, Awaitable && ... awaitable);

        template<class Awaiter, class TimePoint, class... Awaitable>
        auto crt_co_post_at(Awaiter && awaiter, TimePoint && timepoint, Awaitable && ... awaitable);

        template<class Awaiter, class TimePoint, class... Awaitable>
        auto crt_co_dispatch_at(Awaiter && awaiter, TimePoint && timepoint, Awaitable && ... awaitable);

        template<class Awaiter, class TimePoint, class... Awaitable>
        auto crt_co_defer_at(Awaiter && awaiter, TimePoint && timepoint, Awaitable && ... awaitable);
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
