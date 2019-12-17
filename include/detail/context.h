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
            executable{ make_xmanaged(executable_t{ std::function{ func } }) },
            action{ _action },
            executor{ _executor }
        {
            task.executee = [this]() { (*executable)(); };
            task.notifier = [this]() { notifyComplete(); };
        }

        void addPrior(context_t* context)
        {
            task.cntPrior.fetch_add(1);
            
            context->handler.push_back([this]() { reflectPrior(); });
        }

        void addPostrior(context_t* context)
        {
            context->task.cntPrior.fetch_add(1);

            handler.push_back([&context]() { context->reflectPrior(); });
        }

        void release()
        {
            task.isReleased.store(true);

            if (task.cntPrior.load() == 0)
                task.act(*executor, action);
        }

        void reflectPrior()
        {
            task.cntPrior.fetch_sub(1);

            if (task.isReleased.load() && task.cntPrior.load() == 0)
                task.act(*executor, action);
        }

        void notifyComplete()
        {
            for (auto& f : handler)
                f();
        }

    protected:
        xmanaged_ptr<executable_base_t> executable;
        task_t task;
        action_t action;
        executor_t* executor = nullptr;
        std::deque<std::function<void()>> handler;
    };
}