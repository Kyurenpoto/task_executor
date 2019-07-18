#pragma once

namespace task_executor
{
    inline namespace interface_v1
    {
        struct op_base
        {
            struct op_raw;
            struct op_continuation;
            struct op_wrap;
        };

        struct op_execute : op_base
        {
            template<class T0, class... T>
            static constexpr bool is_op_raw =
                is_executor_v<T0> &&
                sizeof...(T) > 0 &&
                is_executable_v<T>...;

            template<class T0, class... T>
            static constexpr bool is_op_continuation =
                is_executor_v<T0> &&
                sizeof...(T) == 1 &&
                is_executable_continuation_v<T>...;

            template<class T0, class... T>
            static constexpr bool is_op_wrap =
                is_wraped_executable_continuation_v<T0> &&
                is_wraped_executable_continuation_v<T>...;

            template<class T0, class... T>
            using type = std::conditional_t<
                is_op_raw<T0, T...>, op_raw,
                std::conditional_t<is_op_continuation<T0, T...>, op_continuation,
                std::conditional_t<is_op_wrap<T0, T...>, op_wrap, void>>>;
        };

        struct op_await : op_base
        {
            template<class T0, class... T>
            static constexpr bool is_op_raw =
                is_awaiter_v<T0> &&
                sizeof...(T) > 0 &&
                is_awaitable_v<T>...;

            template<class T0, class... T>
            static constexpr bool is_op_continuation =
                is_awaiter_v<T0> &&
                sizeof...(T) == 1 &&
                is_awaitable_continuation_v<T>...;

            template<class T0, class... T>
            static constexpr bool is_op_wrap =
                is_wraped_awaitable_continuation_v<T0> &&
                is_wraped_awaitable_continuation_v<T>...;

            template<class T0, class... T>
            using type = std::conditional_t<
                is_op_raw<T0, T...>, op_raw,
                std::conditional_t<is_op_continuation<T0, T...>, op_continuation,
                std::conditional_t<is_op_wrap<T0, T...>, op_wrap, void>>>;
        };
    }
}

#include "interface/post.h"
#include "interface/dispatch.h"
#include "interface/defer.h"

#include "interface/post_after.h"
#include "interface/dispatch_after.h"
#include "interface/defer_after.h"

#include "interface/post_at.h"
#include "interface/dispatch_at.h"
#include "interface/defer_at.h"

#include "interface/co_post.h"
#include "interface/co_dispatch.h"
#include "interface/co_defer.h"

#include "interface/co_post_after.h"
#include "interface/co_dispatch_after.h"
#include "interface/co_defer_after.h"

#include "interface/co_post_at.h"
#include "interface/co_dispatch_at.h"
#include "interface/co_defer_at.h"
