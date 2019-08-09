#pragma once

#include <atomic>
#include <thread>

#include "crt/utils.h"

namespace task_executor
{
    inline namespace controller_v1
    {
        struct controller
        {
            virtual ~controller() = default;
        };

        struct thread_pool;

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
