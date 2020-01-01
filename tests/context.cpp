#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include "task_executor.h"

TEST_CASE("execute_context")
{
    using namespace task_executor;

    SUBCASE("release_after_notify")
    {
        getThreadLocal().currentExecutor = nullptr;

        std::shared_ptr<strand_executor_t> e =
            std::make_shared<strand_executor_t>();
        
        bool flag1 = false, flag2 = false;
        context_t
            a{ std::function{[&flag1]()->void { flag1 = true; }}, action_t::DISPATCH, &(*e) },
            b{ std::function{[&flag2]()->void { flag2 = true; }}, action_t::DISPATCH, &(*e) };

        a.addPostrior(&b);

        a.release();

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == false);

        b.release();

        REQUIRE(flag2 == true);
    }

    SUBCASE("notify_after_release")
    {
        getThreadLocal().currentExecutor = nullptr;

        std::shared_ptr<strand_executor_t> e =
            std::make_shared<strand_executor_t>();

        bool flag1 = false, flag2 = false;
        context_t
            a{ std::function{[&flag1]()->void { flag1 = true; }}, action_t::DISPATCH, &(*e) },
            b{ std::function{[&flag2]()->void { flag2 = true; }}, action_t::DISPATCH, &(*e) };

        a.addPostrior(&b);

        b.release();

        REQUIRE(flag1 == false);
        REQUIRE(flag2 == false);

        a.release();

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == true);
    }

    SUBCASE("link")
    {
        getThreadLocal().currentExecutor = nullptr;

        std::shared_ptr<strand_executor_t> e =
            std::make_shared<strand_executor_t>();

        bool flag1 = false, flag2 = false;
        int res = 0;
        context_t
            a{ std::function{[&flag1]()->int { flag1 = true; return 1; }}, action_t::DISPATCH, &(*e) },
            b{ std::function{[&flag2, &res](int x)->void { flag2 = true; res = x; }}, action_t::DISPATCH, &(*e) };

        a.addPostrior(&b, transmit_func{
            [](executable_base_t* a, executable_base_t* b)->void {
                executable_t<int()>* realA =
                    static_cast<executable_t<int()>*>(a);
                executable_t<void(int)>* realB =
                    static_cast<executable_t<void(int)>*>(b);

                realB->setArg<0>(realA->getRet());
            } });

        b.release();
        a.release();

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == true);
        REQUIRE(res == 1);
    }
}
