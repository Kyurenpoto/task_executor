#pragma once

#include "crt_struct.h"
#include "executor.h"

namespace task_executor
{
    struct system_executor_t :
        executor_t<system_executor_t>
    {
        template<class Task>
        void assign(Task* task)
        {

        }

        static system_executor_t& get()
        {
            static system_executor_t systemExecutor;

            return systemExecutor;
        }
    };
}
