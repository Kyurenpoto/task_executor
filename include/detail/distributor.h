#pragma once

#include <array>
#include <tuple>
#include <chrono>

#include "task_container.h"

namespace task_executor
{
    struct distributor_t
    {
        void assignImmediate(task_deque*);
        void assignShortTerm(timed_task_map*);
        void assignLongTerm(timed_task_map*);
        task_deque* takeImmediate();
        void renewShortTerm();
        void renewLongTerm(std::size_t idxSlot);

    private:
        crt_list_deque<task_deque*> immediates;
        std::array<crt_list_deque<timed_task_map*>, cntTimeSlot> shortTerms;
        std::array<crt_list_deque<timed_task_map*>, cntTimeSlot> longTerms;
    };

    namespace detail
    {
        distributor_t* getDistributor()
        {
            static distributor_t* distributor = new distributor_t;

            return distributor;
        }
    }
}