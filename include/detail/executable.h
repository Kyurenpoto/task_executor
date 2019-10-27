#pragma once

#include <atomic>
#include <functional>
#include <bitset>
#include <tuple>

#include "util.h"
#include "crt_struct.h"

// transmitter_t, executable_base_t
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
}

// executable_impl_t
namespace task_executor
{
    template<class... Ts>
    struct executable_impl_t;

    template<class Ret, class Arg, class... Args, template<size_t> class BitSet>
    struct executable_impl_t<Ret(Arg, Args...), BitSet<sizeof...(Args) + 2>> final :
        executable_base_t
    {
        template<class Func>
        executable_impl_t(Func func) :
            exe{ func }, args{}, ret{}, checker{}
        {}

        template<std::size_t N, class T>
        void setArg(T&& value) noexcept
        {
            std::get<N>(args) = value;

            checker.set(N + 1);
        }

        const Ret& getRet() const
        {
            if (!checker.test(0))
                throw std::logic_error{ "Return value not recorded" };

            return ret;
        }

    private:
        void execute() override
        {
            executeImpl(std::make_index_sequence<1 + sizeof...(Args)>{});
        }

        template<std::size_t... Ns>
        void executeImpl(std::index_sequence<Ns...>)
        {
            auto argChecker = checker | (BitSet<sizeof...(Args) + 2>{1});
            if (!argChecker.all())
                throw std::logic_error{ "Some arguments not recorded" };

            ret = exe(std::get<Ns>(args)...);

            checker.set(0);
        }

        std::tuple<Arg, Args...> args;
        Ret ret;
        BitSet<sizeof...(Args) + 2> checker;
        std::function<Ret(Arg, Args...)> exe;
    };

    template<class Arg, class... Args, template<size_t> class BitSet>
    struct executable_impl_t<void(Arg, Args...), BitSet<sizeof...(Args) + 1>> final :
        executable_base_t
    {
        template<class Func>
        executable_impl_t(Func func) :
            exe{ func }, args{}, checker{}
        {}

        template<std::size_t N, class T>
        void setArg(T&& value) noexcept
        {
            std::get<N>(args) = value;

            checker.set(N);
        }

    protected:
        void execute() override
        {
            executeImpl(std::make_index_sequence<1 + sizeof...(Args)>{});
        }

        template<std::size_t... Ns>
        void executeImpl(std::index_sequence<Ns...>)
        {
            if (!checker.all())
                throw std::logic_error{ "Some arguments not recorded" };

            exe(std::get<Ns>(args)...);
        }
        std::tuple<Arg, Args...> args;
        BitSet<sizeof...(Args) + 1> checker;
        std::function<void(Arg, Args...)> exe;
    };

    template<class Ret, template<size_t> class BitSet>
    struct executable_impl_t<Ret(), BitSet<1>> final :
        executable_base_t
    {
        template<class Func>
        executable_impl_t(Func func) :
            exe{ func }, ret{}, checker{}
        {}

        const Ret& getRet() const
        {
            if (!checker.test(0))
                throw std::logic_error{ "Return value not recorded" };

            return ret;
        }

    private:
        void execute() override
        {
            ret = exe();

            checker.set(0);
        }

        Ret ret;
        BitSet<1> checker;
        std::function<Ret()> exe;
    };

    template<template<size_t> class BitSet>
    struct executable_impl_t<void(), BitSet<0>> final :
        executable_base_t
    {
        template<class Func>
        executable_impl_t(Func func) :
            exe{ func }
        {}

    private:
        void execute() override
        {
            exe();
        }

        std::function<void()> exe;
    };
}

// executable_t
namespace task_executor
{
    template<class T>
    struct checker_size_t;

    template<class Ret, class Arg, class... Args>
    struct checker_size_t<Ret(Arg, Args...)>
    {
        static constexpr std::size_t value = sizeof...(Args) + 2;
    };

    template<class Arg, class... Args>
    struct checker_size_t<void(Arg, Args...)>
    {
        static constexpr std::size_t value = sizeof...(Args) + 1;
    };

    template<class Ret>
    struct checker_size_t<Ret()>
    {
        static constexpr std::size_t value = 1;
    };

    template<>
    struct checker_size_t<void()>
    {
        static constexpr std::size_t value = 0;
    };

    template<class T>
    constexpr std::size_t CHECKER_SIZE = checker_size_t<T>::value;

    template<class T, class BitSet = crt_bitset<CHECKER_SIZE<T>>>
    using executable_t = typename executable_impl_t<T, BitSet>;
}
