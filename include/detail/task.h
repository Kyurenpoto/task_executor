#pragma once

#include <atomic>
#include <deque>
#include <chrono>
#include <functional>

#include "util.h"
#include "byte_stream.h"

namespace task_executor
{
    struct executable_base_t
    {
        void operator()()
        {
            execute();
            transmit();
        }

        template<class Invokable>
        void link(Invokable && invokable)
        {
            transmitter.push_back(invokable);
        }

        void transmit()
        {
            for (auto& f : transmitters)
                f(this);
        }

        virtual void execute() = 0;

    protected:
        std::deque<std::function<void(executable_base_t*)>> transmitters;
    };

    template<class Ret, class... Args>
    struct executable_t :
        executable_base_t
    {
        virtual void execute() override
        {
            ret = exe(args...);
        }

        byte_stream<Args...> args;
        byte_stream<Ret> ret;
        std::function<Ret(Args...)> exe;
        std::size_t cntReceptedArgs = 0;
    };

    template<class... Args>
    struct executable_t<void, Args...> :
        executable_base_t
    {
        virtual void execute() override
        {
            exe(args...);
        }

        byte_stream<Args...> args;
        std::function<void(Args...)> exe;
        std::size_t cntReceptedArgs = 0;
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
