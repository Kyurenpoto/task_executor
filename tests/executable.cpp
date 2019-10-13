#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include "task_executor.h"

TEST_CASE("execute_executable_no_args_no_ret")
{
    using namespace task_executor;

    bool flag = false;
    executable_t<void()>{ [&flag]() { flag = true; } }();

    REQUIRE(flag == true);
}

TEST_CASE("execute_executable_no_args_1_ret")
{
    using namespace task_executor;

    executable_t<int()> x{ []() { return 1; } };
    x();
    const int& res = x.getRet();

    REQUIRE(res == 1);
}

TEST_CASE("execute_executable_1_args_no_ret")
{
    using namespace task_executor;

    int val = 0;
    executable_t<void(int)> x{ [&val](int a) { val = a; } };
    x.setArg<0>(1);
    x();

    REQUIRE(val == 1);
}

TEST_CASE("execute_executable_2_args_no_ret")
{
    using namespace task_executor;

    int val = 0;
    executable_t<void(int, int)> x{ [&val](int a, int b) { val = a + b; } };
    x.setArg<0>(1);
    x.setArg<1>(2);
    x();

    REQUIRE(val == 3);
}

TEST_CASE("execute_executable_1_args_1_ret")
{
    using namespace task_executor;

    executable_t<int(int)> x{ [](int a) { return a; } };
    x.setArg<0>(1);
    x();
    const int& ret = x.getRet();

    REQUIRE(ret == 1);
}
