#pragma once

#include <memory>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <atomic>

#include "traits.h"

namespace task_executor
{
    inline namespace graph_v1
    {
		struct task_base;

		using task_id = std::size_t;

        struct graph
        {
            const task_id & reserve(std::shared_ptr<task_base>);

            void activate(const task_id &);

            void link(const task_id &, const task_id &);

            void unbind(const task_id &);

            void release(const task_id &);

        private:
			struct graph_edge
			{
				task_id src;
				task_id dst;
			};

			struct edge_pool
			{
			private:
				std::deque<graph_edge> edges;
				std::unordered_map<task_id, std::unordered_set<task_id>> srcMap, dstMap;
			};
            
			struct id_pool
			{
				const task_id & gain();

				void release(const task_id &);

			private:
				std::unordered_set<task_id> reusables;
			};

			struct task_map
			{
			private:
				std::unordered_map<task_id, std::shared_ptr<task_base>> taskMap;
			};
        };
    }
}
