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
        std::deque<task_t*> arrPosterior;
        executable_base_t* executable = nullptr;
        std::atomic_bool isReleased = false;

        template<class Executor>
        void act(Executor& executor, action_t action)
        {
            if (!isReleased.load())
                throw std::logic_error{ "Task context not released" };

            if (cntPrior.load() != 0)
                throw std::logic_error{ "Incomplete prior exists" };

            switch (action)
            {
            case action_t::POST:
                post(executor);
                break;
            case action_t::DEFER:
                defer(executor);
                break;
            case action_t::DISPATCH:
                dispatch(executor);
                break;
            }
        }

    protected:
        template<class Executor>
        void post(Executor& executor)
        {
            // post = graph pop -> global deque push back

            //executor.assign(this);
        }

        template<class Executor>
        void defer(Executor& executor)
        {
            // defer = graph pop -> local deque push back
        }

        template<class Executor>
        void dispatch(Executor& executor)
        {
            // graph pop -> local deque push front

            executor.assign_front(this);
        }
    };
}
