#pragma once

#include <memory_resource>
#include <chrono>

namespace task_executor
{
    struct executor_base_t;

    struct thread_local_t
    {
        static thread_local
            std::pmr::unsynchronized_pool_resource resource;
        inline static thread_local
            executor_base_t* currentExecutor = nullptr;
        static thread_local
            std::chrono::steady_clock::duration limitExecutionTime;
        inline static thread_local bool haveExecuteTask = false;
    };
}
