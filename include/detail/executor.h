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
    // for task tests
    struct executor_base_t
    {};

    struct executor_t :
        executor_base_t, context_creator_t<executor_t>
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

        virtual void assign_front(task_t* task)
        {
            taskDeq.pushFront(task);
        }

        virtual void assign_back(task_t* task)
        {
            taskDeq.pushBack(task);
        }

        void flush()
        {
            bool oldIsFlushing;
            do
            {
                oldIsFlushing = isFlushing.load();
            } while (!oldIsFlushing &&
                !isFlushing.compare_exchange_weak(oldIsFlushing, true));

            if (oldIsFlushing)
            {
                size_t oldRemainRef;
                do
                {
                    oldRemainRef = remainRef.load();
                } while (oldRemainRef > 0 &&
                    !remainRef.compare_exchange_weak(
                        oldRemainRef, oldRemainRef - 1));

                if (oldRemainRef > 0)
                    flushStealer();

                return;
            }

            flushOwner();

            isFlushing.store(false);
        }

        virtual void flushOwner()
        {
            while (true)
            {
                task_t* task = taskDeq.popFront();
                if (task == nullptr)
                    break;

                executeTask(task);
            }
        }

        virtual void flushStealer()
        {
            task_t* task = taskDeq.popBack();
            if (task != nullptr)
                executeTask(task);
        }

        void executeTask(task_t* task)
        {
            thread_local_t::haveExecuteTask = true;

            task->executee();
            task->notifier();
        }

    private:
        static constexpr size_t SIZE_DEQUE = (1 << 20);

        crt_fixed_deque<task_t, SIZE_DEQUE> taskDeq;
        std::atomic_bool isFlushing = false;
        std::atomic_size_t remainRef;
    };
}