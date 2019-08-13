#pragma once

#include <atomic>
#include <thread>
#include <array>

#include "crt/crt_queue.h"

namespace task_executor
{
    inline namespace controller_v1
    {
        struct fixed_task_deque
        {
        private:
            task_base * ptr = nullptr;
            std::atomic<std::size_t> front = 0;
            std::atomic<std::size_t> back = 0;
        };

        struct array_pool
        {

        };

        struct fixed_deque_pool
        {

        };

        struct controller
        {
            virtual ~controller() = default;

            crt_queue<fixed_task_deque *> dequeList;
        };

        struct task_base;

        struct system final :
            controller
        {
            system(const system&) = delete;
            system(system&&) = delete;
            system& operator= (const system&) = delete;
            system& operator= (system&&) = delete;

            system() = default;
            ~system() = default;

        private:
			using distributor = crt_queue<fixed_task_deque *>;

            struct thread_property
            {
                static constexpr std::size_t limitFailureBlocked = 5;
                static constexpr std::size_t timeBlocking = 10;

                std::size_t cntFailureBlocked = 0;
                bool isAlive = false;
            };

            auto & getThreadPropertyList()
            {
                static atomic_monotic_list<thread_property> list;

                return list;
            }

            thread_property * getThreadProperty()
            {
                static thread_local thread_property * tp = nullptr;

                if (tp == nullptr)
                {
                    tp = new thread_property;

                    getThreadPropertyList().push(tp);
                }

                return tp;
            }
        };

        struct thread_pool final :
            controller
        {
            thread_pool(const thread_pool &) = delete;
            thread_pool(thread_pool &&) = delete;
            thread_pool & operator= (const thread_pool &) = delete;
            thread_pool & operator= (thread_pool &&) = delete;

            thread_pool(std::size_t reqThread) :
                reqThread{ reqThread }
            {}

        private:
            system s;
            std::size_t reqThread;
        };
    }
}
