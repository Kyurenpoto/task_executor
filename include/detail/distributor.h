#pragma once

#include <array>
#include <tuple>
#include <chrono>

#include "constant.h"
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

        void assignContext(xmanaged_ptr<context_t>&& context)
        {

        }

        void flushImmediate(
            std::chrono::steady_clock::duration& limitExecutionTime)
        {
            std::chrono::steady_clock::duration totalExecutionTime{ 0 };
            while (limitExecutionTime > totalExecutionTime)
            {
                thread_local_t::haveExecuteTask = false;

                std::chrono::steady_clock::time_point start =
                    std::chrono::steady_clock::now();

                xmanaged_ptr<context_t>* context = immediates.popFront();
                if (context == nullptr)
                    break;

                (*context)->release();

                std::chrono::steady_clock::time_point end =
                    std::chrono::steady_clock::now();

                if (thread_local_t::haveExecuteTask)
                    totalExecutionTime += (end - start);
            }

            limitExecutionTime -=
                std::min(limitExecutionTime, totalExecutionTime);
        }

    private:
        static constexpr size_t SIZE_DEQUE = (1 << 20);

        inline static std::atomic_bool hasOwner = false;

        crt_fixed_deque<xmanaged_ptr<context_t>, SIZE_DEQUE> immediates;

        void assignImmediate()
        {
        }

        void assignShortTerm()
        {
        }

        void assignLongTerm()
        {
        }

        void updateShortTerm()
        {
        }

        void updateLongTerm()
        {
        }
    };

    distributor_t& getDistributor()
    {
        return distributor_t::get();
    }
}
