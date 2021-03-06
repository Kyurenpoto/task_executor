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
        context_t(Func&& func,
            action_t _action,
            executor_t* _executor,
            time_point_t _timePoint = getEpoch()) :
            executable{ make_xmanaged(executable_t{ std::function{ func } }) },
            action{ _action },
            timePoint{ _timePoint },
            executor{ _executor }
        {
            task.executee = [this]()
            {
                std::this_thread::sleep_until(timePoint);

                (*executable)();
            };
            task.notifier = [this]() { notifyComplete(); };
        }

        void addPrior(context_t* context)
        {
            task.cntPrior.fetch_add(1);
            
            context->handler.push_back([this]() { reflectPrior(); });
        }

        void addPrior(context_t* context, transmit_func func)
        {
            addPrior(context);

            context->executable->link(&(*executable), func);
        }

        void addPostrior(context_t* context)
        {
            context->task.cntPrior.fetch_add(1);

            handler.push_back([context]() { context->reflectPrior(); });
        }

        void addPostrior(context_t* context, transmit_func func)
        {
            addPostrior(context);

            executable->link(&(*(context->executable)), func);
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

            isCompleted.store(true);
        }

        const std::chrono::steady_clock::time_point getTimePoint() const noexcept
        {
            return timePoint;
        }

        const bool getIsCompleted() const noexcept
        {
            return isCompleted.load();
        }

    protected:
        xmanaged_ptr<executable_base_t> executable;
        task_t task;
        action_t action;
        time_point_t timePoint;
        executor_t* executor = nullptr;
        std::deque<std::function<void()>> handler;
        std::atomic_bool isCompleted = false;
    };
}