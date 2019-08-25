#pragma once

#include <memory>
#include <unordered_map>
#include <atomic>

#include "traits.h"

namespace task_executor
{
    inline namespace graph_v1
    {
        struct task_id;

        struct graph
        {
            template<class Callable, class... Args>
            const task_id & reserve(Callable &&, Args && ...);

            void activate(const task_id &);

            void link(const task_id &, const task_id &);

            void unbind(const task_id &);

            void release(const task_id &);

        private:
            struct id_pool;
        };
    }
}
