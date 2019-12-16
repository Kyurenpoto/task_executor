#pragma once

#include <deque>
#include <chrono>

#include "thread_local.h"
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
        std::function<void()> executee;
        std::function<void()> notifier;
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
            executor.assign_back(this);

            if (isSameObj(thread_local_t::currentExecutor, &executor))
                executor.flush();
        }

        template<class Executor>
        void dispatch(Executor& executor)
        {
            if (thread_local_t::currentExecutor != nullptr &&
                !isSameObj(thread_local_t::currentExecutor, &executor))
                throw std::logic_error{ "External dispatch operations are"
                " only allowed from the system executor" };

            executor_base_t* tmp = thread_local_t::currentExecutor;
            thread_local_t::currentExecutor = &executor;

            executor.assign_front(this);
            executor.flush();

            thread_local_t::currentExecutor = tmp;
        }
    };
}
