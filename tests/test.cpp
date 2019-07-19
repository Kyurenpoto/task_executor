#include "gtest/gtest.h"

#include "../include/task_executor.h"

#include <future>
#include <experimental/coroutine>
#include <experimental/generator>

std::experimental::generator<int> temp()
{
    co_yield 1;
}

struct test_t
{
    std::future<int> operator()();

    std::future<int> func(int);
};

TEST(test, test_func)
{
    auto tmp = []()->std::future<int> { co_return 1; };
    constexpr bool val1 = task_executor::is_awaitable_v<decltype(temp)>;
    constexpr bool val2 = task_executor::is_awaitable_v<decltype(tmp)>;
    constexpr bool val3 = task_executor::is_awaitable_v<test_t>;
    constexpr bool val4 = task_executor::is_awaitable_v<decltype(&test_t::func)>;
}
