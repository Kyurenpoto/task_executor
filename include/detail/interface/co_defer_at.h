#pragma once

#include "../traits.h"

namespace task_executor
{
    inline namespace interface_v1
    {
        struct op_co_defer_at : op_await
        {
            struct op_raw
            {
                template<class TimePoint, class Awaiter, class... Awaitable>
                static await_result_t<awaitable_continuation_wrap_t<
                    Awaiter, awaitable_continuation_t<Awaitable...>>>
                    invoke(TimePoint &&, Awaiter &&, Awaitable && ...)
                {
                    static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                        "It doesn't satisfy awaiter traits or awaitable traits");

                    return std::declval<await_result_t<awaitable_continuation_wrap_t<
                        Awaiter, awaitable_continuation_t<Awaitable...>>>>();
                }
            };

            struct op_continuation
            {
                template<class TimePoint, class Awaiter, class AwaitableContinuation>
                static await_result_t<awaitable_continuation_wrap_t<
                    Awaiter, AwaitableContinuation>>
                    invoke(TimePoint &&, Awaiter &&, AwaitableContinuation &&)
                {
                    static_assert(is_awaiter_v<Awaiter> &&
                        is_awaitable_continuation_v<AwaitableContinuation>,
                        "It doesn't satisfy awaiter traits or awaitable continuation traits");

                    return std::declval<await_result_t<awaitable_continuation_wrap_t<
                        Awaiter, AwaitableContinuation>>>();
                }
            };

            struct op_wrap
            {
                template<class TimePoint, class... WrapedAwaitableContinuation>
                static await_result_t<WrapedAwaitableContinuation...>
                    invoke(TimePoint &&, WrapedAwaitableContinuation && ...)
                {
                    static_assert(is_wraped_awaitable_continuation_v<WrapedAwaitableContinuation>...,
                        "It doesn't satisfy wraped awaitable continuation traits");

                    return std::declval<await_result_t<WrapedAwaitableContinuation...>>();
                }
            };
        };

        template<class T, class... U>
        auto co_defer_at(T && arg, U && ... args)
        {
            return op_co_defer_at::type<U...>::invoke(arg, args...);
        }
    }
}
