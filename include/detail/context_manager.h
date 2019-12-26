#pragma once

#include "context.h"

namespace task_executor
{
	struct context_pool_t
	{
		struct element
		{
			bool isAvailable = false;
			xmanaged_ptr<context_t> context;
		};

		std::atomic_bool isActive = true;
		context_pool_t* next = nullptr;
		memory_pool_t* memPool;

		~context_pool_t()
		{
			for (auto& x : contexts)
				x.context = xmanaged_ptr<context_t>{};
		}

		bool isEmpty()
		{
			return contexts.empty();
		}

		void leaveOwner()
		{
			isActive.store(false);
		}

		void assign(xmanaged_ptr<context_t>&& context)
		{
			for (auto& x : contexts)
				if (x.isAvailable)
				{
					x.isAvailable = false;
					x.context = std::move(context);

					return;
				}

			contexts.push_back(element{.context = std::move(context) });
		}

		void flush()
		{
			for (auto& x : contexts)
				if (!x.isAvailable && x.context->getIsCompleted())
				{
					x.isAvailable = true;
					x.context = xmanaged_ptr<context_t>{};
				}
		}

	private:
		std::pmr::deque<element> contexts;
	};

	struct context_manager_t
	{
		std::atomic<context_pool_t*> head;

		~context_manager_t()
		{
			for (auto pool = head.load(); pool != nullptr; pool = head.load())
			{
				head.store(pool->next);

				delete pool;
			}
		}

		context_pool_t* obtain()
		{
			context_pool_t* old = obtainOld();
			if (old != nullptr)
				return old;

			context_pool_t* newHead = new context_pool_t;
			newHead->memPool = getMemoryPool();

			context_pool_t* oldHead;
			do
			{
				oldHead = head.load();
				newHead->next = oldHead;
			} while (!head.compare_exchange_weak(oldHead, newHead));

			return newHead;
		}

	private:
		context_pool_t* obtainOld()
		{
			for (auto pool = head.load(); pool != nullptr; pool = pool->next)
			{
				if (pool->memPool != getMemoryPool())
					continue;
				
				bool oldIsActive;
				do
				{
					oldIsActive = pool->isActive.load();
				} while (oldIsActive ||
					!pool->isActive.compare_exchange_weak(oldIsActive, true));

				return pool;
			}

			return nullptr;
		}
	};

	context_manager_t& getContextManager()
	{
		static context_manager_t manager;

		return manager;
	}

	context_pool_t* getContextPool()
	{
		static thread_local context_pool_t* pool = nullptr;

		if (pool == nullptr)
			pool = getContextManager().obtain();

		return pool;
	}

	template<class Func>
	static xmanaged_ptr<context_t>& createContext(Func&& func,
		action_t action,
		executor_t* executor,
		time_point_t timePoint = getEpoch())
	{

		xmanaged_ptr<context_t> context =
			make_xmanaged<context_t>(func, action, executor, timePoint);
		getContextPool()->assign(context);

		return context;
	}

	void flushContexts()
	{
		getContextPool()->flush();
	}

	void leaveContextPool()
	{
		getContextPool()->leaveOwner();
	}
}
