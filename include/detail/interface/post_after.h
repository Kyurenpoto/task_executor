#pragma once

#include "../traits.h"

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Duration, class Executor, class... Executable>
        execution_result_t<executable_continuation_wrap_t<
            Executor, executable_continuation_t<Executable...>>>
            post_after(Executor&&, Duration&&, Executable&& ...)
        {
            static_assert(is_executor_v<Executor> && is_executable_v<Executable>...,
                "It doesn't satisfy executor traits or executable traits");

            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, executable_continuation_t<Executable...>>>>();
        }

        template<class Duration, class Executor, class ExecutableContinuation>
        execution_result_t<executable_continuation_wrap_t<
            Executor, ExecutableContinuation>>
            post_after(Executor&&, Duration&&, ExecutableContinuation&&)
        {
            static_assert(is_executor_v<Executor> &&
                is_executable_continuation_v<ExecutableContinuation>,
                "It doesn't satisfy executor traits or executable continuation traits");

            return std::declval<execution_result_t<executable_continuation_wrap_t<
                Executor, ExecutableContinuation>>>();
        }

        template<class Duration, class... WrapedExecutableContinuation>
        execution_result_t<WrapedExecutableContinuation...>
            post_after(Duration&&, WrapedExecutableContinuation&& ...)
        {
            static_assert(is_wraped_executable_continuation_v<WrapedExecutableContinuation>...,
                "It doesn't satisfy wraped executable continuation traits");

            return std::declval<execution_result_t<WrapedExecutableContinuation...>>();
        }
    }
}
