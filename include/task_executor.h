#pragma once

namespace task_executor
{
    inline namespace detail
    {
        template<class Executor, class Callable>
        typename Executor::result_type post(Executor &&, Callable &&);

        template<class Executor, class Callable>
        typename Executor::result_type dispatch(Executor&&, Callable&&);

        template<class Executor, class Callable>
        typename Executor::result_type defer(Executor&&, Callable&&);
    }
}
