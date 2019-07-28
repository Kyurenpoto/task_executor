#pragma once

#include <memory>
#include <unordered_map>
#include <atomic>

#include "traits.h"

namespace task_executor
{
    inline namespace graph_v1
    {
        template<class Alloc>
        struct task_base;

        template<class Alloc>
        struct graph
        {
            using shared_task = std::shared_ptr<task_base<Alloc>>;

            void linkDependency(shared_task before, shared_task after);

            void removeTask(shared_task task);

        private:
            std::pmr::unordered_map<typename shared_task::id_type, shared_task> tasks;
        };
    }
}
