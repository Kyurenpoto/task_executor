#pragma once

#include <memory>
#include <atomic>
#include <variant>
#include <optional>
#include <tuple>
#include <functional>

#include "hazard_pointer.h"

namespace task_executor
{
	inline namespace crt_hash_table_v1
	{
		template<class T>
		struct bucket_data_t
		{
			std::atomic_bool active = false;
			std::variant<std::monostate, T> data = std::monostate{};
		};

		template<class T>
		struct bucket_t
		{
			void init(const std::size_t len, std::pmr::unsynchronized_pool_resource * resource);

			void release(const std::size_t len, std::pmr::unsynchronized_pool_resource * resource);

			T & read(const std::size_t index);
			
			template<class U>
			void write(const std::size_t index, U && data);
			
			template<class U>
			std::optional<std::size_t> find(const std::size_t len, U && data);

		private:
			bucket_data_t * dataArr = nullptr;
		};

		template<class T>
		struct bucket_table_t
		{
		private:
			bucket_t * bucket = nullptr;
		};
	}
}
