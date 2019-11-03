#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include "task_executor.h"

#include <memory_resource>

namespace test_task
{
    struct tmp_executor :
        task_executor::executor_base_t
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

TEST_CASE("execute_task")
{
    using namespace task_executor;
    using namespace test_task;

    SUBCASE("1_executor_2_linked_task_dispatch")
    {
        thread_local_t::currentExecutor = nullptr;

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
        b.act(e, action_t::DISPATCH);

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == true);
    }

    SUBCASE("2_executor_separate_2_linked_task_dispatch")
    {
        thread_local_t::currentExecutor = nullptr;

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

        tmp_executor e1, e2;

        a.act(e1, action_t::DISPATCH);
        b.act(e2, action_t::DISPATCH);

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == true);
    }

    SUBCASE("1_executor_act_in_task_dispatch")
    {
        thread_local_t::currentExecutor = nullptr;

        task_t a, b;
        a.isReleased.store(true);
        b.isReleased.store(true);

        tmp_executor e;

        bool flag = false;
        executable_t<void()>
            x{ [&e, &b]()->void { b.act(e, action_t::DISPATCH); } },
            y{ [&flag]()->void { flag = true; } };
        a.executable = &x;
        b.executable = &y;

        a.act(e, action_t::DISPATCH);
        
        e.flush();

        REQUIRE(flag == true);
    }

    SUBCASE("2_executor_act_in_task_with_exception_dispatch")
    {
        thread_local_t::currentExecutor = nullptr;

        task_t a, b;
        a.isReleased.store(true);
        b.isReleased.store(true);

        tmp_executor e1, e2;

        executable_t<void()>
            x{ [&e2, &b]()->void { b.act(e2, action_t::DISPATCH); } },
            y{ []()->void {} };
        a.executable = &x;
        b.executable = &y;

        REQUIRE_THROWS_AS(a.act(e1, action_t::DISPATCH), const std::logic_error&);
    }

    SUBCASE("2_executor_act_in_task_dispatch_defer")
    {
        thread_local_t::currentExecutor = nullptr;

        task_t a, b;
        a.isReleased.store(true);
        b.isReleased.store(true);

        tmp_executor e1, e2;

        executable_t<void()>
            x{ [&e2, &b]()->void { b.act(e2, action_t::DEFER); } },
            y{ []()->void {} };
        a.executable = &x;
        b.executable = &y;

        REQUIRE_NOTHROW(a.act(e1, action_t::DISPATCH));
    }
}

TEST_CASE("execute_task_with_multi_thread")
{
    using namespace task_executor;

    SUBCASE("")
    {
        SUBCASE("")
        {
            std::pmr::unsynchronized_pool_resource res;
            std::pmr::deque<task_t> taskDeq
            { std::pmr::polymorphic_allocator<task_t>{ &res } };
        }
    }
}
