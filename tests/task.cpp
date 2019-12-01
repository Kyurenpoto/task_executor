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
        
        e2.flush();
    }
}

namespace test_task
{
    struct tmp_executor_lock_free :
        task_executor::executor_base_t
    {
        void assign_front(task_executor::task_t* task)
        {
            taskDeq.pushFront(task);
        }

        void assign_back(task_executor::task_t* task)
        {
            taskDeq.pushBack(task);
        }

        void flush()
        {
            if (isFlushing.load())
            {
                flushStealer();
                return;
            }

            isFlushing.store(true);

            flushOwner();

            isFlushing.store(false);
        }

        void flushOwner()
        {
            while (true)
            {
                task_executor::task_t* task = taskDeq.popFront();
                if (task == nullptr)
                    break;

                executeTask(task);
            }
        }

        void flushStealer()
        {
            task_executor::task_t* task = taskDeq.popBack();
            if (task != nullptr)
                executeTask(task);
        }

        void executeTask(task_executor::task_t* task)
        {
            (*(task->executable))();

            for (auto next : task->arrPosterior)
                next->cntPrior.fetch_sub(1);
        }

        task_executor::crt_fixed_deque<task_executor::task_t, 5> taskDeq;
        std::atomic_bool isFlushing = false;
    };
}

TEST_CASE("execute_task_with_multi_thread")
{
    using namespace task_executor;
    using namespace test_task;

    SUBCASE("1_executor_2_thread_1_context_dispatch")
    {
        task_t a, b;
        a.isReleased.store(true);
        b.isReleased.store(true);
        a.arrPosterior.push_back(&b);
        b.cntPrior.fetch_add(1);

        std::atomic_bool flag1 = false, flag2 = false;
        executable_t<void()> x{ [&flag1]()->void { flag1.store(true); } },
            y{ [&flag2]()->void { flag2.store(true); } };
        a.executable = &x;
        b.executable = &y;

        tmp_executor_lock_free e;

        std::thread t1{ [&e, &flag1, &flag2]() {
            thread_local_t::currentExecutor = nullptr;

            while (!flag1.load() || !flag2.load())
                e.flush();
        } };

        std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });

        std::thread t2{ [&e, &a, &b](){
            thread_local_t::currentExecutor = nullptr;

            a.act(e, action_t::DISPATCH);

            while (b.cntPrior.load() != 0);
            b.act(e, action_t::DISPATCH);
        } };

        t2.join();
        t1.join();

        REQUIRE(flag1.load() == true);
        REQUIRE(flag2.load() == true);
    }

    SUBCASE("1_executor_3_thread_2_context_dispatch")
    {
        task_t a, b, c, d;
        a.isReleased.store(true);
        b.isReleased.store(true);
        c.isReleased.store(true);
        d.isReleased.store(true);
        a.arrPosterior.push_back(&b);
        b.cntPrior.fetch_add(1);
        c.arrPosterior.push_back(&d);
        d.cntPrior.fetch_add(1);

        std::atomic_bool flag1 = false, flag2 = false, flag3 = false, flag4 = false;
        executable_t<void()> x{ [&flag1]()->void { flag1.store(true); } },
            y{ [&flag2]()->void { flag2.store(true); } },
            u{ [&flag3]()->void { flag3.store(true); } },
            v{ [&flag4]()->void { flag4.store(true); } };
        a.executable = &x;
        b.executable = &y;
        c.executable = &u;
        d.executable = &v;

        tmp_executor_lock_free e;

        std::thread t1{ [&e, &flag1, &flag2, &flag3, &flag4]() {
            thread_local_t::currentExecutor = nullptr;

            while (!flag1.load() || !flag2.load() || !flag3.load() || !flag4.load())
                e.flush();
        } };

        std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });

        std::thread t2{ [&e, &a, &b]() {
            thread_local_t::currentExecutor = nullptr;

            a.act(e, action_t::DISPATCH);

            while (b.cntPrior.load() != 0);
            b.act(e, action_t::DISPATCH);
        } };

        std::thread t3{ [&e, &c, &d]() {
            thread_local_t::currentExecutor = nullptr;

            c.act(e, action_t::DISPATCH);

            while (d.cntPrior.load() != 0);
            d.act(e, action_t::DISPATCH);
        } };

        t2.join();
        t3.join();
        t1.join();

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == true);
        REQUIRE(flag3 == true);
        REQUIRE(flag4 == true);
    }
    
    SUBCASE("1_executor_3_thread_2_context_dispatch_cross")
    {
        task_t a, b, c, d;
        a.isReleased.store(true);
        b.isReleased.store(true);
        c.isReleased.store(true);
        d.isReleased.store(true);
        a.arrPosterior.push_back(&b);
        b.cntPrior.fetch_add(1);
        c.arrPosterior.push_back(&d);
        d.cntPrior.fetch_add(1);

        std::atomic_bool flag1 = false, flag2 = false, flag3 = false, flag4 = false;
        executable_t<void()> x{ [&flag1]()->void { flag1.store(true); } },
            y{ [&flag2]()->void { flag2.store(true); } },
            u{ [&flag3]()->void { flag3.store(true); } },
            v{ [&flag4]()->void { flag4.store(true); } };
        a.executable = &x;
        b.executable = &y;
        c.executable = &u;
        d.executable = &v;

        tmp_executor_lock_free e;

        std::thread t1{ [&e, &flag1, &flag2, &flag3, &flag4]() {
            thread_local_t::currentExecutor = nullptr;

            while (!flag1.load() || !flag2.load() || !flag3.load() || !flag4.load())
                e.flush();
        } };

        std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });

        std::thread t2{ [&e, &a, &d]() {
            thread_local_t::currentExecutor = nullptr;

            a.act(e, action_t::DISPATCH);

            while (d.cntPrior.load() != 0);
            d.act(e, action_t::DISPATCH);
        } };

        std::thread t3{ [&e, &c, &b]() {
            thread_local_t::currentExecutor = nullptr;

            c.act(e, action_t::DISPATCH);

            while (b.cntPrior.load() != 0);
            b.act(e, action_t::DISPATCH);
        } };

        t2.join();
        t3.join();
        t1.join();

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == true);
        REQUIRE(flag3 == true);
        REQUIRE(flag4 == true);
    }

    SUBCASE("1_executor_3_thread_2_context_dispatch_split")
    {
        task_t a, b, c, d;
        a.isReleased.store(true);
        b.isReleased.store(true);
        c.isReleased.store(true);
        d.isReleased.store(true);
        a.arrPosterior.push_back(&b);
        b.cntPrior.fetch_add(1);
        c.arrPosterior.push_back(&d);
        d.cntPrior.fetch_add(1);

        std::atomic_bool flag1 = false, flag2 = false, flag3 = false, flag4 = false;
        executable_t<void()> x{ [&flag1]()->void { flag1.store(true); } },
            y{ [&flag2]()->void { flag2.store(true); } },
            u{ [&flag3]()->void { flag3.store(true); } },
            v{ [&flag4]()->void { flag4.store(true); } };
        a.executable = &x;
        b.executable = &y;
        c.executable = &u;
        d.executable = &v;

        tmp_executor_lock_free e;

        std::thread t1{ [&e, &flag1, &flag2, &flag3, &flag4]() {
            thread_local_t::currentExecutor = nullptr;

            while (!flag1.load() || !flag2.load() || !flag3.load() || !flag4.load())
                e.flush();
        } };

        std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });

        std::thread t2{ [&e, &a, &c]() {
            thread_local_t::currentExecutor = nullptr;

            a.act(e, action_t::DISPATCH);

            c.act(e, action_t::DISPATCH);
        } };

        std::thread t3{ [&e, &b, &d]() {
            thread_local_t::currentExecutor = nullptr;

            while (b.cntPrior.load() != 0);
            b.act(e, action_t::DISPATCH);

            while (d.cntPrior.load() != 0);
            d.act(e, action_t::DISPATCH);
        } };

        t2.join();
        t3.join();
        t1.join();

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == true);
        REQUIRE(flag3 == true);
        REQUIRE(flag4 == true);
    }
    
    SUBCASE("1_executor_3_thread_act_in_task_dispatch_defer")
    {
        task_t a, b, c, d;
        a.isReleased.store(true);
        b.isReleased.store(true);
        c.isReleased.store(true);
        d.isReleased.store(true);

        tmp_executor_lock_free e;

        std::atomic_bool flag1 = false, flag2 = false;
        executable_t<void()>
            x{ [&e, &b]()->void { b.act(e, action_t::DEFER); } },
            y{ [&flag1]()->void { flag1.store(true); } },
            u{ [&e, &d]()->void { d.act(e, action_t::DEFER); } },
            v{ [&flag2]()->void { flag2.store(true); } };
        a.executable = &x;
        b.executable = &y;
        c.executable = &u;
        d.executable = &v;

        std::thread t1{ [&e, &flag1, &flag2]() {
            thread_local_t::currentExecutor = nullptr;

            while (!flag1.load() || !flag2.load())
                e.flush();
        } };

        std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });

        std::thread t2{ [&e, &a, &b]() {
            thread_local_t::currentExecutor = nullptr;

            a.act(e, action_t::DISPATCH);

            e.flush();
        } };

        std::thread t3{ [&e, &c, &d]() {
            thread_local_t::currentExecutor = nullptr;

            c.act(e, action_t::DISPATCH);

            e.flush();
        } };

        t2.join();
        t3.join();
        t1.join();

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == true);
    }
    
    SUBCASE("2_executor_3_thread_1_context_respectively_dispatch")
    {
        task_t a, b;
        a.isReleased.store(true);
        b.isReleased.store(true);
        a.arrPosterior.push_back(&b);
        b.cntPrior.fetch_add(1);

        std::atomic_bool flag1 = false, flag2 = false;
        executable_t<void()> x{ [&flag1]()->void { flag1.store(true); } },
            y{ [&flag2]()->void { flag2.store(true); } };
        a.executable = &x;
        b.executable = &y;

        tmp_executor_lock_free e1, e2;

        std::thread t1{ [&e1, &e2, &flag1, &flag2]() {
            thread_local_t::currentExecutor = nullptr;

            while (!flag1.load() || !flag2.load())
            {
                e1.flush();
                e2.flush();
            }
        } };

        std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });

        std::thread t2{ [&e1, &a]() {
            thread_local_t::currentExecutor = nullptr;

            a.act(e1, action_t::DISPATCH);
        } };

        std::thread t3{ [&e2, &b]() {
            thread_local_t::currentExecutor = nullptr;

            while (b.cntPrior.load() != 0);
            b.act(e2, action_t::DISPATCH);
        } };

        t2.join();
        t3.join();
        t1.join();

        REQUIRE(flag1.load() == true);
        REQUIRE(flag2.load() == true);
    }
    
    SUBCASE("2_executor_3_thread_2_context_respectively_cross_dispatch")
    {
        task_t a, b, c, d;
        a.isReleased.store(true);
        b.isReleased.store(true);
        c.isReleased.store(true);
        d.isReleased.store(true);
        a.arrPosterior.push_back(&b);
        b.cntPrior.fetch_add(1);
        c.arrPosterior.push_back(&d);
        d.cntPrior.fetch_add(1);

        tmp_executor_lock_free e1, e2;

        std::atomic_bool flag1 = false, flag2 = false, flag3 = false, flag4 = false;
        executable_t<void()>
            x{ [&flag1]()->void { flag1.store(true); } },
            y{ [&flag2]()->void { flag2.store(true); } },
            u{ [&flag3]()->void { flag3.store(true); } },
            v{ [&flag4]()->void { flag4.store(true); } };
        a.executable = &x;
        b.executable = &y;
        c.executable = &u;
        d.executable = &v;

        std::thread t1{ [&e1, &e2, &flag1, &flag2, &flag3, &flag4]() {
            thread_local_t::currentExecutor = nullptr;

            while (!flag1.load() || !flag2.load() || !flag3.load() || !flag4.load())
            {
                e1.flush();
                e2.flush();
            }
        } };

        std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });

        std::thread t2{ [&e1, &a, &d]() {
            thread_local_t::currentExecutor = nullptr;

            a.act(e1, action_t::DISPATCH);

            while (d.cntPrior.load() != 0);
            d.act(e1, action_t::DISPATCH);
        } };

        std::thread t3{ [&e2, &c, &b]() {
            thread_local_t::currentExecutor = nullptr;

            c.act(e2, action_t::DISPATCH);

            while (b.cntPrior.load() != 0);
            b.act(e2, action_t::DISPATCH);
        } };

        t2.join();
        t3.join();
        t1.join();

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == true);
        REQUIRE(flag3 == true);
        REQUIRE(flag4 == true);
    }
    
    SUBCASE("2_executor_3_thread_act_in_task_cross_dispatch_defer")
    {
        task_t a, b, c, d;
        a.isReleased.store(true);
        b.isReleased.store(true);
        c.isReleased.store(true);
        d.isReleased.store(true);

        tmp_executor_lock_free e1, e2;

        std::atomic_bool flag1 = false, flag2 = false;
        executable_t<void()>
            x{ [&e2, &b]()->void { b.act(e2, action_t::DEFER); } },
            y{ [&flag1]()->void { flag1.store(true); } },
            u{ [&e1, &d]()->void { d.act(e1, action_t::DEFER); } },
            v{ [&flag2]()->void { flag2.store(true); } };
        a.executable = &x;
        b.executable = &y;
        c.executable = &u;
        d.executable = &v;

        std::thread t1{ [&e1, &e2, &flag1, &flag2]() {
            thread_local_t::currentExecutor = nullptr;

            while (!flag1.load() || !flag2.load())
            {
                e1.flush();
                e2.flush();
            }
        } };

        std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });

        std::thread t2{ [&e1, &a, &b]() {
            thread_local_t::currentExecutor = nullptr;

            a.act(e1, action_t::DISPATCH);

            e1.flush();
        } };

        std::thread t3{ [&e2, &c, &d]() {
            thread_local_t::currentExecutor = nullptr;

            c.act(e2, action_t::DISPATCH);

            e2.flush();
        } };

        t2.join();
        t3.join();
        t1.join();

        REQUIRE(flag1 == true);
        REQUIRE(flag2 == true);
    }
}
