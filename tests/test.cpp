#include "gtest/gtest.h"

#include <future>
#include <experimental/coroutine>
#include <experimental/generator>
#include <functional>
#include <algorithm>
#include <execution>
#include <vector>
#include <atomic>

#include "task_executor.h"

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

void func(std::vector<int> & v)
{
    std::atomic<int> sum = 0;
    std::for_each(std::execution::par_unseq, v.begin(), v.end(), [&sum](int x)
        {
            sum.fetch_add(x, std::memory_order_relaxed);
        });
}

TEST(test, test_func)
{
    using type = decltype(&test_t::func);
    constexpr bool val5 = std::is_same_v<type, std::future<int>(test_t::*)(int)>;
}
