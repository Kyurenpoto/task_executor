#pragma once

#include "crt_struct.h"
#include "executor.h"

namespace task_executor
{
    // backward push, forward pop
    template<
        class TaskDeque = task_deque,
        class TimedTaskMap = timed_task_map
    >
    struct strand_executor_t :
        executor_t<strand_executor_t<TaskDeque, TimedTaskMap>,
        TaskDeque, TimedTaskMap>
    {
        template<class Task>
        void assign(Task* task)
        {

        }
    };
}
