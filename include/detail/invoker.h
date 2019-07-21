#pragma once

#include <future>
#include <tuple>

#include "traits.h"
#include "callable.h"

namespace task_executor
{
    inline namespace invoker_v1
    {
        enum class invoker_t
        {
            seq_exe,
            seq_awt,
            par_exe,
            par_awt
        };

        template<invoker_t, class... T>
        struct invoker;

        template<class... Executable>
        struct invoker<invoker_t::seq_exe, Executable...>
        {
            invoker(Executable && ...);

            void invoke();
            auto get_future();

        private:
            template<class T>
            using return_type = typename function_traits<T>::return_type;

            template<class T>
            using invoker_type = invoker<invoker_t::seq_exe, T>;

            size_t idx;
            size_t cntComplete;
            std::promise<std::tuple<return_type<Executable>...>> promise;
            std::tuple<std::future<return_type<Executable>>...> futures;
            std::tuple<invoker_type<Executable>...> invokers;
        };

        template<class Executable>
        struct invoker<invoker_t::seq_exe, Executable>
        {
            invoker(Executable &&);

            void invoke();
            auto get_future();

        private:
            using return_type = typename function_traits<Executable>::return_type;

            std::promise<return_type> promise;
            callable<Executable> executable;
        };

        template<class... Awaitable>
        struct invoker<invoker_t::seq_awt, Awaitable...>;

        template<class... Executable>
        struct invoker<invoker_t::par_exe, Executable...>;

        template<class... Awaitable>
        struct invoker<invoker_t::par_awt, Awaitable...>;
    }
}
