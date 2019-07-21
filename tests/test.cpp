#include "gtest/gtest.h"

#include "../include/task_executor.h"

#include <future>
#include <experimental/coroutine>
#include <experimental/generator>
#include <functional>

using tttt = std::packaged_task<void()>;

std::experimental::generator<int> temp()
{
    co_yield 1;
}

struct test_t
{
    std::future<int> operator()()
    {
        return x.get_future();
    }

    std::future<int> func(int)
    {
        return x.get_future();
    }

    std::promise<int> x;
};

TEST(test, test_func)
{
    auto tmp = []()->std::future<int> { co_return 1; };
    constexpr bool val1 = task_executor::is_awaitable_v<decltype(temp)>;
    constexpr bool val2 = task_executor::is_awaitable_v<decltype(tmp)>;
    constexpr bool val3 = task_executor::is_awaitable_v<test_t>;
    constexpr bool val4 = task_executor::is_awaitable_v<decltype(&test_t::func)>;

    using type = decltype(&test_t::func);
    constexpr bool val5 = std::is_same_v<type, std::future<int>(test_t::*)(int)>;

    test_t t;
    auto f = task_executor::make_callable(&t, &test_t::func);
}
