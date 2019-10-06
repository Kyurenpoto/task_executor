#pragma once

#include <atomic>
#include <deque>
#include <chrono>
#include <functional>

#include "util.h"
#include "byte_stream.h"

namespace task_executor
{
    struct executable_base_t;
    
    using transmit_func =
        std::function<void(executable_base_t*, executable_base_t*)>;

    struct transmitter_t
    {
        void operator()(executable_base_t* from)
        {
            for (auto& [to, func] : targets)
                func(from, to);
        }

        void addTarget(executable_base_t* to, transmit_func func)
        {
            targets.emplace_back(std::make_pair(to, func));
        }

    private:
        std::deque<std::pair<executable_base_t*, transmit_func>> targets;
    };

    struct executable_base_t
    {
        void operator()()
        {
            execute();
            transmit();
        }

        void link(executable_base_t* target, transmit_func func)
        {
            transmitter.addTarget(target, func);
        }

    private:
        virtual void execute() = 0;

        void transmit()
        {
            transmitter(this);
        }

        transmitter_t transmitter;
    };

    template<class Ret, class... Args>
    struct executable_t final :
        executable_base_t
    {
        executable_t(std::function<Ret(Args...)> func) :
            exe{ func }
        {}

        template<std::size_t N, class T>
        void setArg(T&& value)
        {
            static_assert(N < sizeof...(Args));
            static_assert(std::is_convertible_v<T, nth_type_t<N, Args...>>);

            args.get<N>() = static_cast<nth_type_t<N, Args...>>(value);
        }

        const Ret& getRet() const
        {
            return ret.get<0>();
        }

    private:
        void execute() override
        {
            ret = exe(args...);
        }

        byte_stream<Args...> args;
        byte_stream<Ret> ret;
        std::function<Ret(Args...)> exe;
        std::size_t cntReceptedArgs = 0;
    };

    template<class... Args>
    struct executable_t<void, Args...> final :
        executable_base_t
    {
        executable_t(std::function<void(Args...)> func) :
            exe{ func }
        {}

        template<std::size_t N, class T>
        void setArg(T&& value)
        {
            static_assert(N < sizeof...(Args));
            static_assert(std::is_convertible_v<T, nth_type_t<N, Args...>>);

            args.get<N>() = static_cast<nth_type_t<N, Args...>>(value);
        }

    protected:
        void execute() override
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
