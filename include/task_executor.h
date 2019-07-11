#pragma once

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Executor, class Callable>
        typename Executor::result_type<Callable> post(Executor &&, Callable &&);

        template<class Executor, class Callable>
        typename Executor::result_type<Callable> dispatch(Executor &&, Callable &&);

        template<class Executor, class Callable>
        typename Executor::result_type<Callable> defer(Executor &&, Callable &&);
    }

    inline namespace executor_v1
    {
        struct system;
        
        struct thread_pool;
        
        struct strand;
    }

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
}
