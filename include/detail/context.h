#pragma once

#include "context_creator.h"
#include "executor.h"

namespace task_executor
{
    struct context_t :
        context_creator_t<context_t>
    {
        void addPrior(std::initializer_list<context_t*> contexts)
        {
            for (auto x : contexts)
                x->task->arrPosterior.push_back(task);
        }

        void addPostrior(std::initializer_list<context_t*> contexts)
        {
            for (auto x : contexts)
                task->arrPosterior.push_back(x->task);
        }

        void release(executor_t& executorRequested)
        {
            task->isReleased.store(true);

            if (task->cntPrior.load() == 0)
                task->act(executorRequested, action);
            else
                executor = &executorRequested;
        }

        void reflectPrior()
        {
            task->cntPrior.fetch_sub(1);

            if (task->isReleased.load() && task->cntPrior.load() == 0)
                task->act(*executor, action);
        }

    protected:
        task_t* task;
        action_t action;
        executor_t* executor;
    };
}