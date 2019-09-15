#pragma once

#include <tuple>
#include <chrono>

#include "crt_struct.h"

namespace task_executor
{
    struct task_actor_t;

    struct immediate_distributor_t
    {
        void add(std::tuple<crt_list_deque<task_actor_t*>,
            std::atomic_bool, std::atomic_size_t>* immediate)
        {
            immediates.pushBack(immediate);
        }
        
        std::tuple<crt_list_deque<task_actor_t*>,
            std::atomic_bool, std::atomic_size_t>* take()
        {
            if (immediates.isEmpty())
                return nullptr;
            else
                return immediates.popFront();
        }

    private:
        crt_list_deque<std::tuple<crt_list_deque<task_actor_t*>,
            std::atomic_bool, std::atomic_size_t>*>
            immediates;
    };

    namespace detail
    {
        immediate_distributor_t* getImmediateDistributor()
        {
            static immediate_distributor_t* distributor =
                new immediate_distributor_t;

            return distributor;
        }
    }

    struct short_term_distributor_t
    {
        void add(crt_map<std::chrono::steady_clock::time_point, task_actor_t*>*
            shortTerm)
        {
            shortTerms.pushBack(shortTerm);
        }

    private:
        crt_list_deque<
            crt_map<std::chrono::steady_clock::time_point, task_actor_t*>*>
            shortTerms;
    };

    namespace detail
    {
        short_term_distributor_t* getShortTermDistributor()
        {
            static short_term_distributor_t* distributor =
                new short_term_distributor_t;

            return distributor;
        }
    }

    struct long_term_distributor_t
    {
        void add(crt_map<std::chrono::steady_clock::time_point, task_actor_t*>*
            longTerm)
        {
            longTerms.pushBack(longTerm);
        }

    private:
        crt_list_deque<
            crt_map<std::chrono::steady_clock::time_point, task_actor_t*>*>
            longTerms;
    };

    namespace detail
    {
        long_term_distributor_t* getLongTermDistributor()
        {
            static long_term_distributor_t* distributor =
                new long_term_distributor_t;

            return distributor;
        }
    }
}