#pragma once

#include "crt_struct.h"
#include "executor.h"

namespace task_executor
{
    struct thread_pool_executor_t :
        executor_t<thread_pool_executor_t>
    {
        thread_pool_executor_t(const size_t nThreads) :
            executor_t<thread_pool_executor_t>{ nThreads - 1 }
        {
            static_assert(nThreads >= 1);
        }
    };
}
