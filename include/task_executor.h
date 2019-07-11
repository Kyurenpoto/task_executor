#pragma once

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Executor, class Executable>
        typename Executor::result_type<Executable> post(Executor &&, Executable &&);

        template<class Executor, class Executable>
        typename Executor::result_type<Executable> dispatch(Executor &&, Executable &&);

        template<class Executor, class Executable>
        typename Executor::result_type<Executable> defer(Executor &&, Executable &&);
    }

    inline namespace executor_v1
    {
        inline namespace async_executor_v1
        {
            struct async_executor;

            template<class Owner>
            struct async_executor_impl;

            template<class Owner>
            struct local_async_executor : async_executor_impl<Owner>;

            template<class Owner>
            struct global_async_executor : async_executor_impl<Owner>;

            struct async_executor_distributor;
        }

        struct system;

        struct thread_pool;

        struct strand;
    }
}
