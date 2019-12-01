#pragma once

#include <initializer_list>
#include <tuple>
#include <array>

#include "context_creator.h"
#include "util.h"
#include "crt_struct.h"
#include "task.h"

namespace task_executor
{
    struct task_t;

    struct executor_base_t :
        context_creator_t<executor_base_t>
    {

    };

    /*
      backward push/pop, basically
      forward push/pop, without ownership
    */

    template<class T>
    struct executor_t :
        executor_base_t
    {
        executor_t(const size_t maxRef) :
            remainRef{ maxRef }
        {}

        template<class Context>
        void release(std::initializer_list<Context*> contexts)
        {
            for (auto x : contexts)
                x->release(this);
        }

        void assign_front(task_t* task)
        {
            taskDeq.pushFront(task);
        }

        void assign_back(task_t* task)
        {
            taskDeq.pushBack(task);
        }

        void flush()
        {
            if (isFlushing.load())
            {
                flushStealer();
                return;
            }

            isFlushing.store(true);

            flushOwner();

            isFlushing.store(false);
        }

        void flushOwner()
        {
            while (true)
            {
                task_executor::task_t* task = taskDeq.popFront();
                if (task == nullptr)
                    break;

                executeTask(task);
            }
        }

        void flushStealer()
        {
            size_t oldRemainRef;
            do
            {
                oldRemainRef = remainRef.load();
                if (oldRemainRef == 0)
                    return;
            } while (!remainRef.compare_exchange_weak(
                oldRemainRef, oldRemainRef - 1));

            task_executor::task_t* task = taskDeq.popBack();
            if (task != nullptr)
                executeTask(task);
        }

        void executeTask(task_executor::task_t* task)
        {
            (*(task->executable))();

            for (auto next : task->arrPosterior)
                next->cntPrior.fetch_sub(1);
        }

    private:
        static constexpr size_t SIZE_DEQUE = 100;

        crt_fixed_deque<task_t, SIZE_DEQUE> taskDeq;
        std::atomic_bool isFlushing = false;
        std::atomic_size_t remainRef;
    };

    // Facade struct that can use executor through inheritance
}