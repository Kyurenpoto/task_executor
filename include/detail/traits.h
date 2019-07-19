#pragma once

#include <type_traits>
#include <experimental/coroutine>

namespace task_executor
{
    inline namespace traits_v1
    {
        template<class T>
        struct function_traits_impl;

        template<class R, class... A>
        struct function_traits_impl<R(A...)>
        {
            using return_type = R;
            using args_type = std::tuple<A...>;
        };

        template<class R, class... A>
        function_traits_impl<R(A...)> function_traits_helper(R(*)(A...))
        {
            return std::declval<function_traits_impl<R(A...)>>();
        }

        template<class T, class R, class... A>
        function_traits_impl<R(A...)> function_traits_helper(R(T::*)(A...))
        {
            return std::declval<function_traits_impl<R(A...)>>();
        }

        template<class T, class R, class... A>
        function_traits_impl<R(A...)> function_traits_helper(R(T::*)(A...) const)
        {
            return std::declval<function_traits_impl<R(A...)>>();
        }

        template<class T, class R, class... A>
        function_traits_impl<R(A...)> function_traits_helper(R(T::*)(A...) volatile)
        {
            return std::declval<function_traits_impl<R(A...)>>();
        }

        template<class T, class R, class... A>
        function_traits_impl<R(A...)> function_traits_helper(R(T::*)(A...) const volatile)
        {
            return std::declval<function_traits_impl<R(A...)>>();
        }

        template<class T>
        auto function_traits_helper(T &&)
        {
            return function_traits_helper(&T::operator());
        }

        template<class T>
        using function_traits = decltype(function_traits_helper(std::declval<T>()));

        template<class T>
        struct is_executor
        {
            template<class, class>
            class checker;

            class dummy1
            {};

            class dummy2
            {};
            
            template<class U>
            static std::true_type test_post(checker<U,
                decltype(std::declval<U>().post(dummy1{}, dummy2{}))> *);
            template<class U>
            static std::false_type test_post(...);

            template<class U>
            static std::true_type test_dispatch(checker<U,
                decltype(std::declval<U>().dispatch(dummy1{}, dummy2{}))> *);
            template<class U>
            static std::false_type test_dispatch(...);

            template<class U>
            static std::true_type test_defer(checker<U,
                decltype(std::declval<U>().defer(dummy1{}, dummy2{}))> *);
            template<class U>
            static std::false_type test_defer(...);

            static constexpr bool value =
                std::is_same_v<std::true_type, decltype(test_post<T>(nullptr))> &&
                std::is_same_v<std::true_type, decltype(test_dispatch<T>(nullptr))> &&
                std::is_same_v<std::true_type, decltype(test_defer<T>(nullptr))> &&;
        };

        template<class T>
        inline constexpr bool is_executor_v =
            is_executor<T>::value;

        template<class T>
        using is_executable =
            std::is_function<std::remove_pointer_t<T>>;

        template<class Executable>
        inline constexpr bool is_executable_v =
            is_executable<Executable>::value;

        template<class T>
        struct is_awaiter
        {
            template<class, class>
            class checker;

            class dummy1
            {};

            class dummy2
            {};

            template<class U>
            static std::true_type test_co_post(checker < U,
                decltype(std::declval<U>().co_post(dummy1{}, dummy2{})) > *);
            template<class U>
            static std::false_type test_co_post(...);

            template<class U>
            static std::true_type test_co_dispatch(checker < U,
                decltype(std::declval<U>().co_dispatch(dummy1{}, dummy2{})) > *);
            template<class U>
            static std::false_type test_co_dispatch(...);

            template<class U>
            static std::true_type test_co_defer(checker < U,
                decltype(std::declval<U>().co_defer(dummy1{}, dummy2{})) > *);
            template<class U>
            static std::false_type test_co_defer(...);

            static constexpr bool value =
                std::is_same_v<std::true_type, decltype(test_co_post<T>(nullptr))>&&
                std::is_same_v<std::true_type, decltype(test_co_dispatch<T>(nullptr))>&&
                std::is_same_v<std::true_type, decltype(test_co_defer<T>(nullptr))> &&;
        };

        template<class T>
        inline constexpr bool is_awaiter_v =
            is_awaiter<T>::value;

        template<class T>
        struct is_awaitable
        {
            using return_type = typename function_traits<T>::return_type;

            using promise_type = typename
                std::experimental::coroutine_traits<return_type>::promise_type;

            static std::true_type test(promise_type *);
            static std::false_type test(...);

            static constexpr bool value = std::is_same_v<std::true_type, decltype(test(nullptr))>;
        };

        template<class T>
        inline constexpr bool is_awaitable_v =
            is_awaitable<T>::value;

        template<class ExecutableContinuation, class = void>
        struct is_executable_continuation : std::false_type
        {};

        template<class ExecutableContinuation>
        inline constexpr bool is_executable_continuation_v =
            is_executable_continuation<ExecutableContinuation>::value;

        template<class AwaitableContinuation, class = void>
        struct is_awaitable_continuation : std::false_type
        {};

        template<class AwaitableContinuation>
        inline constexpr bool is_awaitable_continuation_v =
            is_awaitable_continuation<AwaitableContinuation>::value;

        template<class WrapedExecutableContinuation, class = void>
        struct is_wraped_executable_continuation : std::false_type
        {};

        template<class WrapedExecutableContinuation>
        inline constexpr bool is_wraped_executable_continuation_v =
            is_wraped_executable_continuation<WrapedExecutableContinuation>::value;

        template<class WrapedAwaitableContinuation, class = void>
        struct is_wraped_awaitable_continuation : std::false_type
        {};

        template<class WrapedAwaitableContinuation>
        inline constexpr bool is_wraped_awaitable_continuation_v =
            is_wraped_awaitable_continuation<WrapedAwaitableContinuation>::value;
    }
}