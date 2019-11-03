#pragma once

#include "crt_struct.h"
#include "executor.h"

namespace task_executor
{
    template<
        class TaskDeque = task_deque,
        class TimedTaskMap = timed_task_map
    >
    struct system_executor_t :
        executor_t<system_executor_t<TaskDeque, TimedTaskMap>,
        TaskDeque, TimedTaskMap>
    {
        template<class Task>
        void assign(Task* task)
        {

        }

        static system_executor_t<TaskDeque, TimedTaskMap>& get()
        {
            static system_executor_t<TaskDeque, TimedTaskMap> systemExecutor;

            return systemExecutor;
        }
    };
}
