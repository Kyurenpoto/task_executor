#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include "task_executor.h"

namespace test_strand
{
    struct test_strand_convert :
        task_executor::strand_executor_t
    {
        void assign_back(task_executor::task_t* task) override
        {
            callAssignBack.store(true);

            task_executor::strand_executor_t::assign_back(task);
        }

        std::atomic_bool callAssignBack = false;
    };

    struct test_strand_no_steal :
        task_executor::strand_executor_t
    {
        void flushOwner() override
        {

        }

        void flushStealer() override
        {
            hasEverStolen.store(true);
        }

        std::atomic_bool hasEverStolen = false;
    };
}

TEST_CASE("strand_executor")
{
    using namespace task_executor;
    using namespace test_strand;

    SUBCASE("convert_dispatch_to_defer")
    {
        std::shared_ptr<test_strand_convert> s =
            std::make_shared<test_strand_convert>();

        task_t t;
        t.isReleased.store(true);

        t.executee = []() {};
        t.notifier = []() {};

        t.act(*s, action_t::DISPATCH);

        REQUIRE(s->callAssignBack.load() == true);
    }

    SUBCASE("no_stealing")
    {
        std::shared_ptr<test_strand_no_steal> s =
            std::make_shared<test_strand_no_steal>();
        auto f = [&s]() {
            for (int i = 0; i < 100'000; ++i)
                s->flush();
        };

        std::thread t1{ f };
        std::thread t2{ f };
        std::thread t3{ f };
        std::thread t4{ f };
        std::thread t5{ f };
        std::thread t6{ f };
        std::thread t7{ f };
        std::thread t8{ f };

        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();
        t6.join();
        t7.join();
        t8.join();

        REQUIRE(s->hasEverStolen.load() == false);
    }
}
