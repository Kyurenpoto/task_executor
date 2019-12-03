#pragma once

#include "crt_struct.h"
#include "executor.h"

namespace task_executor
{
    struct strand_executor_t :
        executor_t
    {
        strand_executor_t() :
            executor_t{ 0 }
        {}

        void assign_front(task_t* task) override
        {
            assign_back(task);
        }
    };
}
