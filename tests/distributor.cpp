#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include "task_executor.h"

TEST_CASE("execute_distributor")
{
    using namespace task_executor;

    SUBCASE("immediate_context")
    {
        getThreadLocal().currentExecutor = nullptr;

        std::shared_ptr<strand_executor_t> e =
            std::make_shared<strand_executor_t>();

        bool flag = false;
        auto c = make_xmanaged<context_t>(std::function{ [&flag]()->void { flag = true; } }, action_t::DISPATCH, &(*e));

        distributor_t::assignContext(c);

        duration_t d = std::chrono::seconds{ 10 };

        distributor_t::flushImmediate(d);

        REQUIRE(flag == true);
    }

    SUBCASE("timed_context_short")
    {
        getThreadLocal().currentExecutor = nullptr;

        std::shared_ptr<strand_executor_t> e =
            std::make_shared<strand_executor_t>();

        auto timing = std::chrono::steady_clock::now() + std::chrono::milliseconds{ 30 };

        bool flag = false;
        auto c = make_xmanaged<context_t>(std::function{ [&flag]()->void { flag = true; } }, action_t::DISPATCH, &(*e),
            timing);

        distributor_t::assignContext(c);

        duration_t d = std::chrono::seconds{ 10 };
        distributor_t::flushImmediate(d);

        REQUIRE(flag == false);

        std::this_thread::sleep_until(timing);
        distributor_t::enterTimedLoop(true);

        distributor_t::flushImmediate(d);

        REQUIRE(flag == true);
    }

    SUBCASE("timed_context_long")
    {
        getThreadLocal().currentExecutor = nullptr;

        std::shared_ptr<strand_executor_t> e =
            std::make_shared<strand_executor_t>();

        auto timing = std::chrono::steady_clock::now() + std::chrono::milliseconds{ 2020 };

        bool flag = false;
        auto c = make_xmanaged<context_t>(std::function{ [&flag]()->void { flag = true; } }, action_t::DISPATCH, &(*e),
            timing);

        distributor_t::assignContext(c);

        duration_t d = std::chrono::seconds{ 10 };
        distributor_t::flushImmediate(d);

        REQUIRE(flag == false);

        std::this_thread::sleep_for(std::chrono::milliseconds{ 40 });
        distributor_t::enterTimedLoop(true);

        distributor_t::flushImmediate(d);

        REQUIRE(flag == false);

        distributor_t::enterTimedLoop(true);

        distributor_t::flushImmediate(d);

        REQUIRE(flag == false);

        std::this_thread::sleep_until(timing);
        distributor_t::enterTimedLoop(true);
        distributor_t::enterTimedLoop(true);

        distributor_t::flushImmediate(d);

        REQUIRE(flag == true);
    }
}
