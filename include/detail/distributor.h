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

        void enterTimedLoop()
        {
            while (tryTimedLoop());
        }

        bool tryTimedLoop()
        {
            distributor_t& distributor = get();

            bool oldHasOwner = distributor.hasOwner.load();
            if (oldHasOwner ||
                !distributor.hasOwner.compare_exchange_weak(
                    oldHasOwner, true))
                return false;

            distributor.updateShortTerm();
            distributor.updateLongTerm();
        }

        void assignContext(xmanaged_ptr<context_t>& context)
        {
            auto duration = durationContext(context);

            if (duration < SIZE_TIME_SLOT)
                immediates.pushBack(&context);
            else if (duration > SIZE_TOTAL_TIME_SLOT)
                longTerms.pushBack(&context);
            else
                shortTerms.pushBack(&context);
        }

        void flushImmediate(
            std::chrono::steady_clock::duration& limitExecutionTime)
        {
            std::chrono::steady_clock::duration totalExecutionTime{ 0 };
            loopContextDeque(immediates,
                [&, this](xmanaged_ptr<context_t>* context)
                {
                    thread_local_t::haveExecuteTask = false;

                    std::chrono::steady_clock::time_point start =
                        std::chrono::steady_clock::now();

                    (*context)->release();

                    std::chrono::steady_clock::time_point end =
                        std::chrono::steady_clock::now();

                    if (thread_local_t::haveExecuteTask)
                        totalExecutionTime += (end - start);

                    return limitExecutionTime > totalExecutionTime;
                });

            limitExecutionTime -=
                std::min(limitExecutionTime, totalExecutionTime);
        }

    private:
        struct compare_context
        {
            bool operator() (const xmanaged_ptr<context_t>* lhs,
                const xmanaged_ptr<context_t>* rhs) const
            {
                return (*lhs)->getTimePoint() < (*rhs)->getTimePoint();
            }
        };

        static constexpr size_t SIZE_DEQUE = (1 << 20);
        static constexpr std::size_t CNT_TIME_SLOT = 100;
        static constexpr std::chrono::steady_clock::duration
            SIZE_TIME_SLOT = std::chrono::milliseconds{ 20 };
        static constexpr std::chrono::steady_clock::duration
            SIZE_TOTAL_TIME_SLOT = SIZE_TIME_SLOT * CNT_TIME_SLOT;

        inline static std::atomic_bool hasOwner = false;

        using context_deque =
            crt_fixed_deque<xmanaged_ptr<context_t>, SIZE_DEQUE>;

        context_deque immediates, shortTerms, longTerms;

        using context_set =
            std::pmr::set<xmanaged_ptr<context_t>*, compare_context>;

        std::array<context_set, CNT_TIME_SLOT> shortTermsSorted;
        context_set longTermsSorted;

        std::chrono::steady_clock::duration durationContext(
            xmanaged_ptr<context_t>& context)
        {
            return context->getTimePoint() - std::chrono::steady_clock::now();
        }

        void updateShortTerm()
        {
        }

        void updateLongTerm()
        {
            loopContextDeque(longTerms,
                [this](xmanaged_ptr<context_t>* context)
                { longTermsSorted.insert(context); return true; });

            if (longTermsSorted.empty() ||
                durationContext((**longTermsSorted.begin())) >
                SIZE_TOTAL_TIME_SLOT)
                return;
        }

        template<class Func>
        void loopContextDeque(context_deque& deque, Func&& func)
        {
            for (auto context = deque.popFront(); context != nullptr;
                context = deque.popFront())
                if (!func(context))
                    break;
        }
    };

    distributor_t& getDistributor()
    {
        return distributor_t::get();
    }
}
