#pragma once

#include "crt_struct.h"
#include "executor.h"

namespace task_executor
{
    template<
        class TaskDeque = task_deque,
        class TimedTaskMap = timed_task_map
    >
    struct thread_pool_executor_t :
        executor_t<thread_pool_executor_t<TaskDeque, TimedTaskMap>,
        TaskDeque, TimedTaskMap>
    {
        template<class Task>
        void assign(Task* task)
        {

        }
    };
}
