#pragma once

#include <atomic>
#include <deque>
#include <chrono>

#include "executor.h"

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

    struct active_type_t
    {
        enum class action_t
        {
            POST,
            DEFER,
            DISPATCH
        };

        action_t action;
        std::chrono::steady_clock::time_point timePoint;
    };

    template<class T>
    struct task_t
    {
        std::atomic_size_t cntPrior = 0;
        std::deque<task_t<T>*> arrPostrior;
        invoker_base_t* invoker = nullptr;
        result_base_t* result = nullptr;
        std::deque<param_base_t*> arrParam;
        active_type_t activeType;
        std::atomic_bool isReleased = false;
        general_executor_t executor = nullptr;

        void act()
        {
            // timer processing

            switch (activeType.action)
            {
            case active_type_t::action_t::POST:
                post();
                break;
            case active_type_t::action_t::DEFER:
                defer();
                break;
            case active_type_t::action_t::DISPATCH:
                dispatch();
                break;
            }
        }

    protected:
        void post()
        {
            getConcrete<T&>(*this).post();
        }

        void defer()
        {
            getConcrete<T&>(*this).defer();
        }

        void dispatch()
        {
            getConcrete<T&>(*this).dispatch();
        }
    };
}
