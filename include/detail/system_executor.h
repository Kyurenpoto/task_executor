#pragma once

#include <limits>

#include "crt_struct.h"
#include "executor.h"
#include "global.h"

namespace task_executor
{
    struct system_executor_t :
        executor_t
    {
        system_executor_t() :
            executor_t{ std::numeric_limits<size_t>::max() }
        {}

        static system_executor_t& get()
        {
            static system_executor_t systemExecutor;

            return systemExecutor;
        }
    };

    system_executor_t& getSystemExecutor()
    {
        return system_executor_t::get();
    }
}
