#pragma once

#include <deque>
#include <chrono>

#include "global.h"
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
                post();
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
        void post()
        {
            thread_local_t& tls = getThreadLocal();
            executor_base_t* tmp = tls.currentExecutor;
            tls.currentExecutor = nullptr;

            defer(getSystemExecutor());

            tls.currentExecutor = tmp;
        }

        template<class Executor>
        void defer(Executor& executor)
        {
            executor.assign_back(this);

            if (isSameObj(getThreadLocal().currentExecutor, &executor))
                executor.flush();
        }

        template<class Executor>
        void dispatch(Executor& executor)
        {
            thread_local_t& tls = getThreadLocal();
            if (tls.currentExecutor != nullptr &&
                !isSameObj(tls.currentExecutor, &executor))
                throw std::logic_error{ "External dispatch operations are"
                " only allowed from the system executor" };

            executor_base_t* tmp = tls.currentExecutor;
            tls.currentExecutor = &executor;

            executor.assign_front(this);
            executor.flush();

            tls.currentExecutor = tmp;
        }
    };
}
