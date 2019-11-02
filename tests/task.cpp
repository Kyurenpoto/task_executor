#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include "task_executor.h"

#include <memory_resource>

namespace test_task
{
    struct tmp_executor
    {
        void assign_front(task_executor::task_t* task)
        {
            taskDeq.push_front(task);
        }

        void assign_back(task_executor::task_t* task)
        {
            taskDeq.push_back(task);
        }

        void flush()
        {
            if (isFlushing)
                return;

            isFlushing = true;

            while (!taskDeq.empty())
            {
                task_executor::task_t* task = taskDeq.front();
                taskDeq.pop_front();

                (*(task->executable))();

                for (auto next : task->arrPosterior)
                    next->cntPrior.fetch_sub(1);
            }

            isFlushing = false;
        }

        std::deque<task_executor::task_t*> taskDeq;
        bool isFlushing = false;
    };
}

TEST_CASE("execute_task_dispatch")
{
    using namespace task_executor;
    using namespace test_task;

    SUBCASE("")
    {
        std::pmr::unsynchronized_pool_resource res;
        std::pmr::deque<task_t> taskDeq
        { std::pmr::polymorphic_allocator<task_t>{ &res } };
    }

    SUBCASE("one_executor_two_linked_task_no_transmit")
    {
        task_t a, b;
        a.isReleased.store(true);
        b.isReleased.store(true);
        a.arrPosterior.push_back(&b);
        b.cntPrior.fetch_add(1);

        bool flag1 = false, flag2 = false;
        executable_t<void()> x{ [&flag1]()->void { flag1 = true; } },
            y{ [&flag2]()->void { flag2 = true; } };
        a.executable = &x;
        b.executable = &y;

        tmp_executor e;

        a.act(e, action_t::DISPATCH);
        e.flush();
        b.act(e, action_t::DISPATCH);
        e.flush();

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == true);
    }
}

TEST_CASE("execute_task_with_multi_thread")
{
    using namespace task_executor;

    SUBCASE("")
    {
    }
}
