#pragma once

#include <initializer_list>
#include <variant>
#include <tuple>
#include <array>

#include "context_creator.h"
#include "crt_struct.h"
#include "util.h"
#include "constant.h"

namespace task_executor
{
    struct task_actor_t;

    /*
      backward push/pop, basically
      forward push/pop, without ownership
    */
    template<class T>
    struct executor_t :
        context_creator_t<executor_t<T>>
    {
        template<class Context>
        void release(std::initializer_list<Context*> contexts)
        {
            for (auto x : contexts)
                x->release(this);
        }

        // push & execute
        template<class Task>
        void assign(Task* task)
        {
            getConcrete<T&>(*this).assign(task);
        }

    private:
        std::tuple<crt_list_deque<task_actor_t*>,
            std::atomic_bool, std::atomic_size_t>*
            immediate;
        crt_map<std::chrono::steady_clock::time_point, task_actor_t*>*
            shortTerm;
        std::array<
            crt_map<std::chrono::steady_clock::time_point, task_actor_t*>*,
            sizeLongTerm>
            longTerm;
    };

    struct thread_pool_executor_t;
    struct strand_executor_t;
    struct system_executor_t;

    using general_executor_t =
        std::variant<
            std::nullptr_t,
            thread_pool_executor_t*,
            strand_executor_t*,
            system_executor_t*
        >;

    // Facade struct that can use executor through inheritance
    struct executable_t
    {

    };
}