#include "gtest/gtest.h"

#include "../include/task_executor.h"

#include <future>
#include <experimental/coroutine>
#include <experimental/generator>

std::experimental::generator<int> temp()
{
    co_yield 1;
}

TEST(test, test_func)
{
    auto tmp = []()->std::future<int> {};
    constexpr bool val = task_executor::is_awaitable_v<decltype(temp)>;
}
