#pragma once

#include "crt_struct.h"
#include "executor.h"

namespace task_executor
{
    // backward push, forward pop
    struct strand_executor_t :
        executor_t<strand_executor_t>
    {
        template<class Task>
        void assign(Task* task)
        {

        }
    };
}
