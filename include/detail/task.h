#pragma once

#include <atomic>
#include <deque>
#include <chrono>

#include "util.h"

namespace task_executor
{
    struct param_base_t;
    struct result_base_t;
    struct invoker_base_t;

    struct param_base_t
    {
        virtual void link(result_base_t*) = 0;
        virtual void pass(result_base_t*) = 0;
        virtual bool isValid() = 0;

    private:
        bool isLinkded = false;
    };

    struct result_base_t
    {
        virtual void link(std::initializer_list<param_base_t*>) = 0;
        virtual void pass() = 0;

    private:
        std::deque<param_base_t*> arrParam;
    };

    struct invoker_base_t
    {
        virtual void invoke(std::deque<param_base_t*>, result_base_t*) = 0;
    };

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
        invoker_base_t* invoker = nullptr;
        result_base_t* result = nullptr;
        std::deque<param_base_t*> arrParam;
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
