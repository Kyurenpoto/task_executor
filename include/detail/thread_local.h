#pragma once

#include <memory_resource>
#include <chrono>

#include "global.h"
#include "memory_manager.h"

namespace task_executor
{
    struct thread_local_t
    {
        executor_base_t* currentExecutor = nullptr;
        bool isOwner = false;
        bool haveExecuteTask = false;

        ~thread_local_t()
        {
            if (currentExecutor != nullptr && isOwner)
                leaveOwner(currentExecutor);

            getMemoryPool()->isActive.store(false);
        }
    };

    thread_local_t& getThreadLocal()
    {
        static thread_local thread_local_t tls;

        return tls;
    }
}
