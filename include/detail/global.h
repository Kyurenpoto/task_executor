#pragma once

namespace task_executor
{
    struct system_executor_t;

    system_executor_t& getSystemExecutor();

    struct distributor_t;

    distributor_t& getDistributor();
}
