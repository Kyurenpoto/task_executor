#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include "task_executor.h"

namespace test_executable
{
    bool flag = false;
    int val = 0;

    void Init()
    {
        flag = false;
        val = 0;
    }

    void NoArgsNoRet()
    {
        flag = true;
    }

    int NoArgsOneRet()
    {
        return 1;
    }

    void OneArgsNoRet(int a)
    {
        val = a;
    }

    void TwoArgsNoRet(int a, int b)
    {
        val = a + b;
    }

    int OneArgsOneRet(int a)
    {
        return a;
    }

    int TwoArgsOneRet(int a, int b)
    {
        return a + b;
    }
}

TEST_CASE("execute_executable")
{
    using namespace task_executor;
    using namespace test_executable;

    SUBCASE("no_args_no_ret")
    {
        Init();

        executable_t<void()>{ NoArgsNoRet }();

        REQUIRE(flag == true);
    }

    SUBCASE("member_no_args_no_ret")
    {
        Init();

        executable_t<void()>{ []() { NoArgsNoRet(); } }();

        REQUIRE(flag == true);
    }

    SUBCASE("no_args_1_ret")
    {
        Init();

        executable_t<int()> x{ NoArgsOneRet };
        x();
        const int& res = x.getRet();

        REQUIRE(res == 1);
    }

    SUBCASE("member_no_args_1_ret")
    {
        Init();

        executable_t<int()> x{ []()->int { return NoArgsOneRet(); } };
        x();
        const int& res = x.getRet();

        REQUIRE(res == 1);
    }

    SUBCASE("1_args_no_ret")
    {
        Init();

        executable_t<void(int)> x{ OneArgsNoRet };
        x.setArg<0>(1);
        x();

        REQUIRE(val == 1);
    }

    SUBCASE("member_1_args_no_ret")
    {
        Init();

        executable_t<void(int)> x{ [](int a) { OneArgsNoRet(a); } };
        x.setArg<0>(1);
        x();

        REQUIRE(val == 1);
    }

    SUBCASE("2_args_no_ret")
    {
        Init();

        executable_t<void(int, int)> x{ TwoArgsNoRet };
        x.setArg<0>(1);
        x.setArg<1>(2);
        x();

        REQUIRE(val == 3);
    }

    SUBCASE("member_2_args_no_ret")
    {
        Init();

        executable_t<void(int, int)> x{ [](int a, int b) { TwoArgsNoRet(a, b); } };
        x.setArg<0>(1);
        x.setArg<1>(2);
        x();

        REQUIRE(val == 3);
    }

    SUBCASE("1_args_1_ret")
    {
        Init();

        executable_t<int(int)> x{ OneArgsOneRet };
        x.setArg<0>(1);
        x();
        const int& ret = x.getRet();

        REQUIRE(ret == 1);
    }

    SUBCASE("member_1_args_1_ret")
    {
        Init();

        executable_t<int(int)> x{ [](int a)->int { return OneArgsOneRet(a); } };
        x.setArg<0>(1);
        x();
        const int& ret = x.getRet();

        REQUIRE(ret == 1);
    }

    SUBCASE("2_args_1_ret")
    {
        Init();

        executable_t<int(int, int)> x{ TwoArgsOneRet };
        x.setArg<0>(1);
        x.setArg<1>(2);
        x();
        const int& ret = x.getRet();

        REQUIRE(ret == 3);
    }

    SUBCASE("member_2_args_1_ret")
    {
        Init();

        executable_t<int(int, int)> x{ [](int a, int b)->int { return TwoArgsOneRet(a, b); } };
        x.setArg<0>(1);
        x.setArg<1>(2);
        x();
        const int& ret = x.getRet();

        REQUIRE(ret == 3);
    }
}
