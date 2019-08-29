#pragma once

#include <memory>
#include <atomic>
#include <variant>
#include <optional>
#include <tuple>
#include <vector>
#include <list>
#include <functional>

#include "hazard_pointer.h"

namespace task_executor
{
	inline namespace crt_hash_table_v1
	{
		template<class T>
		struct crt_hash_table
		{
			template<class U>
			struct hopscotch_bucket;

			using bucket_array = std::vector<hopscotch_bucket<T>>;
			using overflow_list = std::list<T>;
		};

		template<class Key>
		using crt_hash_set = crt_hash_table<Key>;

		template<class Key, class Value>
		using crt_hash_set = crt_hash_table<std::pair<Key, Value>>;
	}
}
