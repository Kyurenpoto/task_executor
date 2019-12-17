#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include "task_executor.h"

TEST_CASE("execute_context")
{
    using namespace task_executor;

    SUBCASE("release_after_notify")
    {
        thread_local_t::currentExecutor = nullptr;

        strand_executor_t e;
        
        bool flag1 = false, flag2 = false;
        context_t
            a{ std::function{[&flag1]()->void { flag1 = true; }}, action_t::DISPATCH, &e },
            b{ std::function{[&flag2]()->void { flag2 = true; }}, action_t::DISPATCH, &e };

        a.addPostrior(&b);

        a.release();

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == false);

        b.release();

        REQUIRE(flag2 == true);
    }

    SUBCASE("notify_after_release")
    {
        thread_local_t::currentExecutor = nullptr;

        strand_executor_t e;

        bool flag1 = false, flag2 = false;
        context_t
            a{ std::function{[&flag1]()->void { flag1 = true; }}, action_t::DISPATCH, &e },
            b{ std::function{[&flag2]()->void { flag2 = true; }}, action_t::DISPATCH, &e };

        a.addPostrior(&b);

        b.release();

        REQUIRE(flag1 == false);
        REQUIRE(flag2 == false);

        a.release();

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == true);
    }
}
