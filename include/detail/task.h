#pragma once

#include <deque>
#include <chrono>

#include "executable.h"

namespace task_executor
{

    enum class action_t
    {
        POST,
        DEFER,
        DISPATCH
    };

    struct task_t
    {
        std::atomic_size_t cntPrior = 0;
        std::deque<task_t*> arrPostrior;
        executable_base_t* executable = nullptr;
        std::atomic_bool isReleased = false;

        template<class Executor>
        void act(Executor& executor, action_t action)
        {
            switch (action)
            {
            case active_type_t::action_t::POST:
                post(executor);
                break;
            case active_type_t::action_t::DEFER:
                defer(executor);
                break;
            case active_type_t::action_t::DISPATCH:
                dispatch(executor);
                break;
            }
        }

    protected:
        template<class Executor>
        void post(Executor& executor)
        {
            executor.assign(this);
        }

        template<class Executor>
        void defer(Executor& executor)
        {
            // try to change owned deque
        }

        template<class Executor>
        void dispatch(Executor& executor)
        {
            // execute immediately
        }
    };
}
