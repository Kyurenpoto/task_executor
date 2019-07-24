#pragma once

#include <future>
#include <tuple>
#include <algorithm>
#include <utility>

#include "traits.h"
#include "callable.h"

namespace task_executor
{
    inline namespace invoker_v1
    {
        namespace detail
        {
            template<class... T, std::size_t... I>
            auto get_futures_impl(std::tuple<std::promise<T>...> promises, std::index_sequence<I...>)
            {
                return std::make_tuple(std::get<I>(promises).get_future());
            }

            template<class... T>
            auto get_futures(std::tuple<std::promise<T>...> promises)
            {
                return get_futures_impl(promises, std::make_index_sequence<sizeof...(T)>{});
            }
        }

        template<class... Executable>
        struct executable_invoker
        {
            executable_invoker(Executable && ... executables) :
                executables{ executables }
            {}

            auto get_futures()
            {
                return detail::get_futures(promises);
            }

            template<class T>
            using return_type = typename function_traits<T>::return_type;

            std::tuple<callable<Executable>...> executables;
            std::tuple<std::promise<return_type<Executable>>...> promises;
        };

        template<class... Awaitable>
        struct awaitable_invoker
        {
            awaitable_invoker();
        };
    }
}
