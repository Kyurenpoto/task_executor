#pragma once

#include <memory_resource>

namespace task_executor
{
    struct executor_base_t;

    struct thread_local_t
    {
        inline static thread_local
            std::pmr::unsynchronized_pool_resource resource;
        inline static thread_local
            executor_base_t* currentExecutor = nullptr;
    };
}
