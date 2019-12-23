#pragma once

#include <array>
#include <set>

#include "global.h"
#include "context.h"

namespace task_executor
{
    struct distributor_t
    {
        static distributor_t& get()
        {
            static distributor_t distributor;

            return distributor;
        }

        static void enterTimedLoop()
        {
            while (tryTimedLoop());
        }

        static bool tryTimedLoop()
        {
            distributor_t& distributor = get();

            bool oldHasOwner = distributor.hasOwner.load();
            if (oldHasOwner ||
                !distributor.hasOwner.compare_exchange_weak(
                    oldHasOwner, true))
                return false;

            std::chrono::steady_clock::time_point now =
                std::chrono::steady_clock::now();

            distributor.updateShortTerm();
            distributor.updateLongTerm();

            std::this_thread::sleep_until(now + SIZE_TIME_SLOT);
        }

        static void assignContext(xmanaged_ptr<context_t>& context)
        {
            distributor_t& distributor = get();

            duration_t duration =
                context->getTimePoint() - std::chrono::steady_clock::now();
            if (duration < SIZE_TIME_SLOT)
                distributor.immediates.pushBack(&context);
            else if (duration > SIZE_TOTAL_TIME_SLOT)
                distributor.longTerms.pushBack(&context);
            else
                distributor.shortTerms.pushBack(&context);
        }

        static void flushImmediate(duration_t& limitExecutionTime)
        {
            distributor_t& distributor = get();

            duration_t totalExecutionTime{ 0 };

            distributor.loopContextDeque(distributor.immediates,
                flush_immediate{
                    .limitExecutionTime = limitExecutionTime,
                    .totalExecutionTime = totalExecutionTime });

            limitExecutionTime -=
                std::min(limitExecutionTime, totalExecutionTime);
        }

    private:
        static constexpr size_t SIZE_DEQUE = (1 << 20);
        static constexpr std::size_t CNT_TIME_SLOT = 100;
        static constexpr std::chrono::steady_clock::duration
            SIZE_TIME_SLOT = std::chrono::milliseconds{ 20 };
        static constexpr std::chrono::steady_clock::duration
            SIZE_TOTAL_TIME_SLOT = SIZE_TIME_SLOT * CNT_TIME_SLOT;

        using context_deque =
            crt_fixed_deque<xmanaged_ptr<context_t>, SIZE_DEQUE>;

        struct compare_context
        {
            bool operator() (const xmanaged_ptr<context_t>* lhs,
                const xmanaged_ptr<context_t>* rhs) const
            {
                return (*lhs)->getTimePoint() < (*rhs)->getTimePoint();
            }
        };

        using context_set =
            std::pmr::set<xmanaged_ptr<context_t>*, compare_context>;

        struct flush_immediate
        {
            duration_t& limitExecutionTime;
            duration_t& totalExecutionTime;

            bool operator()(xmanaged_ptr<context_t>* context)
            {
                thread_local_t::haveExecuteTask = false;

                duration_t executionTime = getExecutionTime(context);

                if (thread_local_t::haveExecuteTask)
                    totalExecutionTime += executionTime;

                return limitExecutionTime > totalExecutionTime;
            }

            duration_t getExecutionTime(xmanaged_ptr<context_t>* context)
            {
                time_point_t start = std::chrono::steady_clock::now();

                (*context)->release();

                time_point_t end = std::chrono::steady_clock::now();

                return end - start;
            }
        };

        struct move_context_to_short_term
        {
            time_point_t& now;
            std::array<context_set, CNT_TIME_SLOT>& sorted;

            bool operator()(xmanaged_ptr<context_t>* context)
            {
                sorted[getIndex((*context)->getTimePoint())]
                    .insert(context);

                return true;
            }

            size_t getIndex(time_point_t timePoint)
            {
                return (timePoint - now).count() / SIZE_TIME_SLOT.count();
            }
        };

        struct move_context_to_long_term
        {
            context_set& sorted;

            bool operator()(xmanaged_ptr<context_t>* context)
            {
                sorted.insert(context);

                return true;
            }
        };

        std::atomic_bool hasOwner = false;
        context_deque immediates, shortTerms, longTerms;
        std::array<context_set, CNT_TIME_SLOT> shortTermsSorted;
        context_set longTermsSorted;

        distributor_t() = default;

        void updateShortTerm()
        {
            std::chrono::steady_clock::time_point now =
                std::chrono::steady_clock::now();
            loopContextDeque(shortTerms,
                move_context_to_short_term{
                    .now = now, .sorted = shortTermsSorted });

            for (size_t i = 0; i < CNT_TIME_SLOT; ++i)
                updateSorted(shortTermsSorted[i], immediates, SIZE_TIME_SLOT);
        }

        void updateLongTerm()
        {
            loopContextDeque(longTerms,
                move_context_to_long_term{ .sorted = longTermsSorted });

            updateSorted(longTermsSorted, shortTerms, SIZE_TOTAL_TIME_SLOT);
        }

        template<class Func>
        void loopContextDeque(context_deque& deque, Func&& func)
        {
            for (auto context = deque.popFront(); context != nullptr;
                context = deque.popFront())
                if (!func(context))
                    break;
        }

        void updateSorted(context_set& sorted,
            context_deque& dest,
            std::chrono::steady_clock::duration sizeMove)
        {
            if (sorted.empty())
                return;

            context_set::iterator endMove{ getEndMove(sorted, sizeMove) };

            for (auto it = sorted.begin(); it != endMove; ++it)
                dest.pushBack(*it);

            sorted.erase(sorted.begin(), endMove);
        }

        context_set::iterator getEndMove(context_set& sorted,
            std::chrono::steady_clock::duration sizeMove)
        {
            xmanaged_ptr<context_t> context{ make_xmanaged<context_t>(
                    std::function{ []() {} },
                    action_t::DISPATCH,
                    (system_executor_t*)(nullptr),
                    std::chrono::steady_clock::now() + sizeMove) };

            return sorted.upper_bound(&context);
        }
    };

    distributor_t& getDistributor()
    {
        return distributor_t::get();
    }
}
