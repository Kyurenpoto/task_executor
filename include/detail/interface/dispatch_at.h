#pragma once

#include "../traits.h"

namespace task_executor
{
    inline namespace interface_v1
    {
        struct op_dispatch_at : op_execute
        {
            struct op_raw
            {
                template<class TimePoint, class Executor, class... Executable>
                static execution_result_t<executable_continuation_wrap_t<
                    Executor, executable_continuation_t<Executable...>>>
                    invoke(Executor &&, TimePoint &&, Executable && ...)
                {
                    static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                        "It doesn't satisfy executor traits or executable traits");

                    return std::declval<execution_result_t<executable_continuation_wrap_t<
                        Executor, executable_continuation_t<Executable...>>>>();
                }
            };

            struct op_continuation
            {
                template<class TimePoint, class Executor, class ExecutableContinuation>
                static execution_result_t<executable_continuation_wrap_t<
                    Executor, ExecutableContinuation>>
                    invoke(Executor &&, TimePoint &&, ExecutableContinuation &&)
                {
                    static_assert(is_executor_v<Executor> &&
                        is_executable_continuation_v<ExecutableContinuation>,
                        "It doesn't satisfy executor traits or executable continuation traits");

                    return std::declval<execution_result_t<executable_continuation_wrap_t<
                        Executor, ExecutableContinuation>>>();
                }
            };

            struct op_wrap
            {
                template<class TimePoint, class... WrapedExecutableContinuation>
                static execution_result_t<WrapedExecutableContinuation...>
                    invoke(TimePoint &&, WrapedExecutableContinuation && ...)
                {
                    static_assert(is_wraped_executable_continuation_v<WrapedExecutableContinuation>...,
                        "It doesn't satisfy wraped executable continuation traits");

                    return std::declval<execution_result_t<WrapedExecutableContinuation...>>();
                }
            };
        };

        template<class... T>
        auto dispatch_at(T && ... args)
        {
            return op_dispatch_at::type<T...>::invoke(args...);
        }
    }
}
