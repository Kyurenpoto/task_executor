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

    SUBCASE("no_args_1_ret")
    {
        Init();

        executable_t<int()> x{ NoArgsOneRet };
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

    SUBCASE("2_args_no_ret")
    {
        Init();

        executable_t<void(int, int)> x{ TwoArgsNoRet };
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
}

TEST_CASE("execute_executable_with_member_functions")
{
    using namespace task_executor;
    using namespace test_executable;

    SUBCASE("no_args_no_ret")
    {
        Init();

        executable_t<void()>{ []() { NoArgsNoRet(); } }();

        REQUIRE(flag == true);
    }

    SUBCASE("no_args_1_ret")
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

        executable_t<void(int)> x{ [](int a) { OneArgsNoRet(a); } };
        x.setArg<0>(1);
        x();

        REQUIRE(val == 1);
    }

    SUBCASE("2_args_no_ret")
    {
        Init();

        executable_t<void(int, int)> x
        { [](int a, int b) { TwoArgsNoRet(a, b); } };
        x.setArg<0>(1);
        x.setArg<1>(2);
        x();

        REQUIRE(val == 3);
    }

    SUBCASE("1_args_1_ret")
    {
        Init();

        executable_t<int(int)> x
        { [](int a)->int { return OneArgsOneRet(a); } };
        x.setArg<0>(1);
        x();
        const int& ret = x.getRet();

        REQUIRE(ret == 1);
    }

    SUBCASE("2_args_1_ret")
    {
        Init();

        executable_t<int(int, int)> x
        { [](int a, int b)->int { return TwoArgsOneRet(a, b); } };
        x.setArg<0>(1);
        x.setArg<1>(2);
        x();
        const int& ret = x.getRet();

        REQUIRE(ret == 3);
    }
}

TEST_CASE("execute_executable_with_throw")
{
    using namespace task_executor;
    using namespace test_executable;

    SUBCASE("no_args_1_ret")
    {
        Init();

        executable_t<int()> x{ NoArgsOneRet };

        REQUIRE_THROWS_AS(x.getRet(), const std::logic_error&);
    }

    SUBCASE("1_args_no_ret")
    {
        Init();

        executable_t<void(int)> x{ OneArgsNoRet };

        REQUIRE_THROWS_AS(x(), const std::logic_error&);
    }

    SUBCASE("2_args_no_ret_unset_0")
    {
        Init();

        executable_t<void(int, int)> x{ TwoArgsNoRet };
        x.setArg<1>(2);

        REQUIRE_THROWS_AS(x(), const std::logic_error&);
    }

    SUBCASE("2_args_no_ret_unset_1")
    {
        Init();

        executable_t<void(int, int)> x{ TwoArgsNoRet };
        x.setArg<0>(1);
        
        REQUIRE_THROWS_AS(x(), const std::logic_error&);
    }

    SUBCASE("2_args_no_ret_unset_all")
    {
        Init();

        executable_t<void(int, int)> x{ TwoArgsNoRet };

        REQUIRE_THROWS_AS(x(), const std::logic_error&);
    }

    SUBCASE("1_args_1_ret_unset_ret")
    {
        Init();

        executable_t<int(int)> x{ OneArgsOneRet };
        x.setArg<0>(1);

        REQUIRE_THROWS_AS(x.getRet(), const std::logic_error&);
    }

    SUBCASE("1_args_1_ret_unset_arg")
    {
        Init();

        executable_t<int(int)> x{ OneArgsOneRet };

        REQUIRE_THROWS_AS(x(), const std::logic_error&);
    }

    SUBCASE("2_args_1_ret_unset_ret")
    {
        Init();

        executable_t<int(int, int)> x{ TwoArgsOneRet };
        x.setArg<0>(1);
        x.setArg<1>(2);

        REQUIRE_THROWS_AS(x.getRet(), const std::logic_error&);
    }

    SUBCASE("2_args_1_ret_unset_arg_1")
    {
        Init();

        executable_t<int(int, int)> x{ TwoArgsOneRet };
        x.setArg<1>(2);

        REQUIRE_THROWS_AS(x(), const std::logic_error&);
    }

    SUBCASE("2_args_1_ret_unset_arg_2")
    {
        Init();

        executable_t<int(int, int)> x{ TwoArgsOneRet };
        x.setArg<0>(1);

        REQUIRE_THROWS_AS(x(), const std::logic_error&);
    }

    SUBCASE("2_args_1_ret_unset_arg_all")
    {
        Init();

        executable_t<int(int, int)> x{ TwoArgsOneRet };

        REQUIRE_THROWS_AS(x(), const std::logic_error&);
    }
}

TEST_CASE("execute_executable_with_transmitter")
{
    using namespace task_executor;
    using namespace test_executable;

	SUBCASE("1_pre_1_post_1_raw_arg")
	{
		executable_t<int()> x{ []()->int { return 1; } };
		executable_t<void(int)> y{ [](int)->void {} };

		transmit_func f{
            [](executable_base_t* a, executable_base_t* b)->void {
    			executable_t<int()>* realA =
                    static_cast<executable_t<int()>*>(a);
    			executable_t<void(int)>* realB =
                    static_cast<executable_t<void(int)>*>(b);
    
    			realB->setArg<0>(realA->getRet());
    		} };

		x.link(&y, f);
		x();
		
		REQUIRE_NOTHROW(y());
	}

    SUBCASE("1_pre_1_post_1_convert_arg")
    {
        int val = 0;

        executable_t<std::pair<int, int>()> x
        { []()->std::pair<int, int> { return {1, 2}; } };
        executable_t<void(int)> y{ [&val](int a)->void { val = a; } };

        transmit_func f{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<std::pair<int, int>()>* realA =
                    static_cast<executable_t<std::pair<int, int>()>*>(a);
                executable_t<void(int)>* realB =
                    static_cast<executable_t<void(int)>*>(b);
    
                realB->setArg<0>(realA->getRet().first);
            } };

        x.link(&y, f);
        x();

        REQUIRE_NOTHROW(y());
        
        REQUIRE(val == 1);
    }

    SUBCASE("2_pre_1_post")
    {
        executable_t<int()> x{ []()->int { return 1; } };
        executable_t<int()> y{ []()->int { return 1; } };
        executable_t<void(int, int)> z{ [](int, int)->void {} };

        x.link(&z, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<int()>* realA =
                    static_cast<executable_t<int()>*>(a);
                executable_t<void(int, int)>* realB =
                    static_cast<executable_t<void(int, int)>*>(b);
    
                realB->setArg<0>(realA->getRet());
            } });
        y.link(&z, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<int()>* realA =
                    static_cast<executable_t<int()>*>(a);
                executable_t<void(int, int)>* realB =
                    static_cast<executable_t<void(int, int)>*>(b);
    
                realB->setArg<1>(realA->getRet());
            } });
        
        x();
        y();

        REQUIRE_NOTHROW(z());
    }

    SUBCASE("1_pre_2_post")
    {
        executable_t<std::pair<int, int>()> x
        { []()->std::pair<int, int> { return {1, 2}; } };
        executable_t<void(int)> y{ [](int)->void {} };
        executable_t<void(int)> z{ [](int)->void {} };

        x.link(&y, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<std::pair<int, int>()>* realA =
                    static_cast<executable_t<std::pair<int, int>()>*>(a);
                executable_t<void(int)>* realB =
                    static_cast<executable_t<void(int)>*>(b);
    
                realB->setArg<0>(realA->getRet().first);
            } });
        x.link(&z, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<std::pair<int, int>()>* realA =
                    static_cast<executable_t<std::pair<int, int>()>*>(a);
                executable_t<void(int)>* realB =
                    static_cast<executable_t<void(int)>*>(b);
    
                realB->setArg<0>(realA->getRet().second);
            } });

        x();

        REQUIRE_NOTHROW(y());

        REQUIRE_NOTHROW(z());
    }

    SUBCASE("2_pre_2_post")
    {
        executable_t<std::pair<int, int>()> u
        { []()->std::pair<int, int> { return {1, 2}; } };
        executable_t<std::pair<int, int>()> v
        { []()->std::pair<int, int> { return {1, 2}; } };
        executable_t<void(int, int)> x{ [](int, int)->void {} };
        executable_t<void(int, int)> y{ [](int, int)->void {} };

        u.link(&x, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<std::pair<int, int>()>* realA =
                    static_cast<executable_t<std::pair<int, int>()>*>(a);
                executable_t<void(int, int)>* realB =
                    static_cast<executable_t<void(int, int)>*>(b);

                realB->setArg<0>(realA->getRet().first);
            } });
        u.link(&y, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<std::pair<int, int>()>* realA =
                    static_cast<executable_t<std::pair<int, int>()>*>(a);
                executable_t<void(int, int)>* realB =
                    static_cast<executable_t<void(int, int)>*>(b);

                realB->setArg<0>(realA->getRet().second);
            } });
        v.link(&x, transmit_func{
                [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<std::pair<int, int>()>* realA =
                    static_cast<executable_t<std::pair<int, int>()>*>(a);
                executable_t<void(int, int)>* realB = 
                    static_cast<executable_t<void(int, int)>*>(b);

                realB->setArg<1>(realA->getRet().first);
            } });
        v.link(&y, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<std::pair<int, int>()>* realA =
                    static_cast<executable_t<std::pair<int, int>()>*>(a);
                executable_t<void(int, int)>* realB =
                    static_cast<executable_t<void(int, int)>*>(b);
    
                realB->setArg<1>(realA->getRet().second);
            } });

        u();
        v();

        REQUIRE_NOTHROW(x());

        REQUIRE_NOTHROW(y());
    }
}

#include <thread>
#include <future>

TEST_CASE("execute_executable_with_transmitter_multi_thread")
{
    using namespace task_executor;
    using namespace test_executable;

    SUBCASE("2_context_no_interference")
    {
        executable_t<int()> x{ []()->int { return 1; } };
        executable_t<int()> y{ []()->int { return 1; } };
        executable_t<void(int, int)> z{ [](int, int)->void {} };

        x.link(&z, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<int()>* realA =
                    static_cast<executable_t<int()>*>(a);
                executable_t<void(int, int)>* realB =
                    static_cast<executable_t<void(int, int)>*>(b);

                realB->setArg<0>(realA->getRet());
            } });
        y.link(&z, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<int()>* realA =
                    static_cast<executable_t<int()>*>(a);
                executable_t<void(int, int)>* realB =
                    static_cast<executable_t<void(int, int)>*>(b);

                realB->setArg<1>(realA->getRet());
            } });

        std::thread t1{ x };
        t1.join();
        
        std::thread t2{ y };
        t2.join();

        REQUIRE_NOTHROW(z());
    }

    SUBCASE("2_context_transmit_args_1_executable")
    {
        executable_t<int()> x{ []()->int { return 1; } };
        executable_t<int()> y{ []()->int { return 1; } };
        executable_t<void(int, int)> z{ [](int, int)->void {} };

        x.link(&z, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<int()>* realA =
                    static_cast<executable_t<int()>*>(a);
                executable_t<void(int, int)>* realB =
                    static_cast<executable_t<void(int, int)>*>(b);

                realB->setArg<0>(realA->getRet());
            } });
        y.link(&z, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<int()>* realA =
                    static_cast<executable_t<int()>*>(a);
                executable_t<void(int, int)>* realB =
                    static_cast<executable_t<void(int, int)>*>(b);

                realB->setArg<1>(realA->getRet());
            } });

        std::thread t1{ x };
        std::thread t2{ y };

        t1.join();
        t2.join();

        REQUIRE_NOTHROW(z());
    }

    SUBCASE("2_context_transmit_args_2_executable")
    {
        executable_t<int()> x{ []()->int { return 1; } };
        executable_t<int()> y{ []()->int { return 1; } };
        executable_t<void(int, int)> u{ [](int, int)->void {} };
        executable_t<void(int, int)> v{ [](int, int)->void {} };

        x.link(&u, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<int()>* realA =
                    static_cast<executable_t<int()>*>(a);
                executable_t<void(int, int)>* realB =
                    static_cast<executable_t<void(int, int)>*>(b);

                realB->setArg<0>(realA->getRet());
            } });
        x.link(&v, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<int()>* realA =
                    static_cast<executable_t<int()>*>(a);
                executable_t<void(int, int)>* realB =
                    static_cast<executable_t<void(int, int)>*>(b);

                realB->setArg<1>(realA->getRet());
            } });
        y.link(&u, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<int()>* realA =
                    static_cast<executable_t<int()>*>(a);
                executable_t<void(int, int)>* realB =
                    static_cast<executable_t<void(int, int)>*>(b);

                realB->setArg<1>(realA->getRet());
            } });
        y.link(&v, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<int()>* realA =
                    static_cast<executable_t<int()>*>(a);
                executable_t<void(int, int)>* realB =
                    static_cast<executable_t<void(int, int)>*>(b);

                realB->setArg<0>(realA->getRet());
            } });

        std::thread t1{ x };
        std::thread t2{ y };

        t1.join();
        t2.join();

        REQUIRE_NOTHROW(u());
        REQUIRE_NOTHROW(v());
    }
}
