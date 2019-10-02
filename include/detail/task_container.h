#pragma once

#include "crt_struct.h"
#include "constant.h"

namespace task_executor
{
    struct task_t;

    using timed_task_map =
        crt_map<std::chrono::steady_clock::time_point, task_t*>;

    struct task_deque
    {
        crt_list_deque<task_t*> tasks;
        std::atomic_bool hasOwner;
        std::atomic_size_t cntRemainingReference;
    };
}
