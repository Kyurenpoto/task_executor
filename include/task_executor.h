#pragma once

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class...>
        struct executor_result_t;

        template<class Executor, class Executable>
        executor_result_t<Executor, Executable> post(Executor &&, Executable &&);

        template<class Executor, class Executable>
        executor_result_t<Executor, Executable> dispatch(Executor &&, Executable &&);

        template<class Executor, class Executable>
        executor_result_t<Executor, Executable> defer(Executor &&, Executable &&);

        template<class...>
        struct awaiter_result_t;

        template<class Awaiter, class Awaitable>
        awaiter_result_t<Awaiter, Awaitable> post(Awaiter &&, Awaitable &&);

        template<class Awaiter, class Awaitable>
        awaiter_result_t<Awaiter, Awaitable> dispatch(Awaiter &&, Awaitable &&);

        template<class Awaiter, class Awaitable>
        awaiter_result_t<Awaiter, Awaitable> defer(Awaiter &&, Awaitable &&);
    }

    inline namespace executor_v1
    {
        inline namespace async_executor_v1
        {
            struct async_executor;

            template<class Owner>
            struct local_async_executor;

            template<class Owner>
            struct global_async_executor;

            struct async_executor_distributor;
        }

        struct system;

        struct thread_pool;

        struct strand;
    }

    inline namespace awaiter_v1
    {

    }
}
