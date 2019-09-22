#pragma once

#include <array>
#include <tuple>
#include <chrono>

#include "task_container.h"

namespace task_executor
{
    struct distributor_t
    {
        void assignImmediate(task_deque* immediate)
        {
            immediates.pushBack(immediate);
        }

        void assignShortTerm(timed_task_map* shortTerm, std::size_t idxSlot)
        {
            shortTerms[idxSlot].pushBack(shortTerm);
        }

        void assignLongTerm(timed_task_map* longTerm, std::size_t idxSlot)
        {
            longTerms[idxSlot].pushBack(longTerm);
        }

        task_deque* takeImmediate()
        {
            return immediates.isEmpty() ? nullptr : immediates.popFront();
        }

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