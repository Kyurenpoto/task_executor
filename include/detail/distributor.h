#pragma once

#include <array>
#include <tuple>
#include <chrono>

#include "task_container.h"

namespace task_executor
{
    namespace detail
    {
        void timed_loop();
    }

    template<
        class TaskDeque = task_deque,
        class TimedTaskMap = timed_task_map,
        template<class> class ListDeque = crt_list_deque
    >
    struct distributor_t
    {
        void assignImmediate(TaskDeque* immediate)
        {
            immediates.pushBack(immediate);
        }

        void assignShortTerm(TimedTaskMap* shortTerm, std::size_t idxSlot)
        {
            cntTotalTimedTask += 1;

            shortTerms[idxSlot].pushBack(shortTerm);
        }

        void assignLongTerm(TimedTaskMap* longTerm, std::size_t idxSlot)
        {
            cntTotalTimedTask += 1;

            longTerms[idxSlot].pushBack(longTerm);
        }

        TaskDeque* takeImmediate()
        {
            return immediates.isEmpty() ? nullptr : immediates.popFront();
        }

        void updateShortTerm()
        {
            for (auto& deq : shortTerms)
                updateShortTerm(deq);
        }

        void updateLongTerm()
        {
            updateLongTerm(longTerms[idxUpdateLongTerm++]);
            idxUpdateLongTerm %= cntTimeSlot;
        }

        std::size_t getCntTotalTimedTask() const
        {
            return cntTotalTimedTask;
        }

    private:
		void updateShortTerm(ListDeque<TimedTaskMap*>& deq)
		{

		}

		void updateLongTerm(ListDeque<TimedTaskMap*>& deq)
		{

		}

        ListDeque<TaskDeque*> immediates;
        std::array<ListDeque<TimedTaskMap*>, cntTimeSlot> shortTerms;
        std::array<ListDeque<TimedTaskMap*>, cntTimeSlot> longTerms;
        std::size_t idxUpdateLongTerm = 0;
        std::size_t cntTotalTimedTask = 0;
    };

    namespace detail
    {
        distributor_t<>* getDistributor()
        {
            static distributor_t<>* distributor = new distributor_t<>;

            return distributor;
        }

        void timed_loop()
        {
            static std::atomic_bool hasOwner = false;

            for (;;)
            {
                bool oldHasOwner = hasOwner.load();

                if (oldHasOwner)
                    return;

                if (hasOwner.compare_exchange_weak(oldHasOwner, true))
                    break;
            }

            auto distributor = getDistributor();

            for (;distributor->getCntTotalTimedTask() != 0;
                std::this_thread::sleep_for(sizeTimeSlot))
            {
                distributor->updateShortTerm();
                distributor->updateLongTerm();
            }

            // post to system_executor
        }
    }
}