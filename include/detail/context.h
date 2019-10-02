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
        
        const std::deque<param_base_t*>& getParamArr()
        {
            return task->arrParam;
        }

        const result_base_t& getResult()
        {
            return *task->result;
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
        void setActiveType(active_type_t activeType)
        {
            task->activeType = activeType;
        }

        void setInvoker(invoker_base_t* invoker)
        {
            task->invoker = invoker;
        }
    };
}