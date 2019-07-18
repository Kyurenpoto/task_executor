#pragma once

#include "../traits.h"

namespace task_executor
{
    inline namespace interface_v1
    {
        struct op_post_after : op_execute
        {
            struct op_raw
            {
                template<class Duration, class Executor, class... Executable>
                static execution_result_t<executable_continuation_wrap_t<
                    Executor, executable_continuation_t<Executable...>>>
                    invoke(Executor &&, Duration &&, Executable && ...)
                {
                    static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                        "It doesn't satisfy executor traits or executable traits");

                    return std::declval<execution_result_t<executable_continuation_wrap_t<
                        Executor, executable_continuation_t<Executable...>>>>();
                }
            };

            struct op_continuation
            {
                template<class Duration, class Executor, class ExecutableContinuation>
                static execution_result_t<executable_continuation_wrap_t<
                    Executor, ExecutableContinuation>>
                    invoke(Executor &&, Duration &&, ExecutableContinuation &&)
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
                template<class Duration, class... WrapedExecutableContinuation>
                static execution_result_t<WrapedExecutableContinuation...>
                    invoke(Duration &&, WrapedExecutableContinuation && ...)
                {
                    static_assert(is_wraped_executable_continuation_v<WrapedExecutableContinuation>...,
                        "It doesn't satisfy wraped executable continuation traits");

                    return std::declval<execution_result_t<WrapedExecutableContinuation...>>();
                }
            };
        };

        template<class... T>
        auto post_after(T && ... args)
        {
            return op_post_after::type<T...>::invoke(args...);
        }
    }
}
