#pragma once

#include "../traits.h"

namespace task_executor
{
    inline namespace interface_v1
    {
        template<class Awaiter, class... Awaitable>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, awaitable_continuation_t<Awaitable...>>>
            co_post(Awaiter&&, Awaitable&& ...)
        {
            static_assert(is_awaiter_v<Awaiter> && is_awaitable_v<Awaitable>...,
                "It doesn't satisfy awaiter traits or awaitable traits");

            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, awaitable_continuation_t<Awaitable...>>>>();
        }

        template<class Awaiter, class AwaitableContinuation>
        await_result_t<awaitable_continuation_wrap_t<
            Awaiter, AwaitableContinuation>>
            co_post(Awaiter&&, AwaitableContinuation&&)
        {
            static_assert(is_awaiter_v<Awaiter> &&
                is_awaitable_continuation_v<AwaitableContinuation>,
                "It doesn't satisfy awaiter traits or awaitable continuation traits");

            co_yield std::declval<await_result_t<awaitable_continuation_wrap_t<
                Awaiter, AwaitableContinuation>>>();
        }

        template<class... WrapedAwaitableContinuation>
        await_result_t<WrapedAwaitableContinuation...>
            co_post(WrapedAwaitableContinuation&& ...)
        {
            static_assert(is_wraped_awaitable_continuation_v<WrapedAwaitableContinuation>...,
                "It doesn't satisfy wraped awaitable continuation traits");

            return std::declval<await_result_t<WrapedAwaitableContinuation...>>();
        }
    }
}
