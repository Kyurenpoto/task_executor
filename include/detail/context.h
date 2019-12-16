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
        context_t(Func&& func, action_t _action, executor_t* _executor) :
            executable{ getMemoryPool()->xnew<executable_t<Func>>(func) },
            task{ getMemoryPool()->xnew<task_t>() },
            action{ _action },
            executor{ _executor }
        {
            task->executee = [&executable]() { (*executable)(); };
            task->notifier = [this]() { notifyComplete(); };
        }

        void addPrior(std::unique_ptr<context_t>& context)
        {
            task->cntPrior.fetch_add(1);
            
            context->handler.push_back([this]() { reflectPrior(); });
        }

        void addPostrior(std::unique_ptr<context_t>& context)
        {
            context->task->cntPrior.fetch_add(1);

            handler.push_back([&context]() { context->reflectPrior(); });
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

        void notifyComplete()
        {
            for (auto& f : handler)
                f();
        }

    protected:
        std::unique_ptr<executable_base_t> executable = nullptr;
        std::unique_ptr<task_t> task = nullptr;
        action_t action;
        executor_t* executor = nullptr;
        std::deque<std::function<void()>> handler;
    };
}