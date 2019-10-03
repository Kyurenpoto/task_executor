#pragma once

#include "context_creator.h"
#include "task.h"

namespace task_executor
{
    struct context_t :
        context_creator_t<context_t>
    {
        void addPrior(std::initializer_list<context_t*> contexts)
        {
            for (auto x : contexts)
                x->task->arrPostrior.push_back(task);
        }

        void addPostrior(std::initializer_list<context_t*> contexts)
        {
            for (auto x : contexts)
                task->arrPostrior.push_back(x->task);
        }

        const executable_base_t* getExecutable() const
        {
            return task->executable;
        }

        template<class Executor>
        void release(Executor& executor)
        {
            for (;;)
            {
                bool isReleased = task->isReleased.load();

                if (isReleased)
                    break;

                if (task->isReleased.compare_exchange_weak(isReleased, true))
                {
                    std::size_t cntPrior = task->cntPrior.load();
                    if (cntPrior == 0)
                        task->act(executor);

                    task = nullptr;

                    break;
                }
            }
        }

    protected:
        task_t* task;
    };

    struct subcontext_t :
        context_t
    {
        void setExecutable(executable_base_t* executable)
        {
            task->executable = executable;
        }
    };
}