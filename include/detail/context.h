#pragma once

#include "context_creator.h"
#include "executor.h"
#include "memory_manager.h"

namespace task_executor
{
    struct context_t :
        context_creator_t<context_t>
    {
        template<class Func>
        static context_t* concrete(Func&& func, action_t action, executor_t* executor)
        {
            return new context_t{ new task_t{ .executable = new executable_t{ func } }, action, executor };
        }

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
        task_t* task = nullptr;
        action_t action;
        executor_t* executor = nullptr;

        context_t(task_t* _task, action_t _action, executor_t* _executor) :
            task{ _task },
            action{ _action },
            executor{ _executor }
        {}
    };
}