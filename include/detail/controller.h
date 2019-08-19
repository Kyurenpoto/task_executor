#pragma once

#include <atomic>
#include <thread>
#include <array>

#include "crt/crt_queue.h"

namespace task_executor
{
    inline namespace controller_v1
    {
        struct task_base;

        struct fixed_task_deque
        {
            static constexpr std::size_t SIZE = 0x20;

            task_base ** ptr = nullptr;
            std::atomic<std::size_t> front = 0;
            std::atomic<std::size_t> back = 0;
        };

        struct fixed_deque_pool
        {
			using deque_ptr = atomic_reusable_list<fixed_task_deque>::atomic_reusable_ptr;

			deque_ptr * loan();
			void payOff(deque_ptr *);

        private:
            static constexpr std::size_t ARRAY_SIZE = 0x100000;
			static constexpr std::size_t DEQUE_SIZE = 0x20;
            static constexpr std::size_t DEQUE_CNT = ARRAY_SIZE / DEQUE_SIZE;

            using array_pool = atomic_monotic_list<std::array<task_base *, ARRAY_SIZE>>;
            using deque_pool = atomic_reusable_list<fixed_task_deque>;

            auto & getArrayPool()
            {
                static array_pool pool;

                return pool;
            }

            auto & getDequePool()
            {
                static deque_pool pool;
                
                return pool;
            }

			void initFixedDeque(fixed_task_deque * fixedDeque)
			{
				static std::atomic<task_base **> origin = nullptr;
				static std::atomic<std::size_t> id = 0;
				static std::atomic<bool> isAlreadyAlloc = false;

				std::size_t idOld = id.fetch_add(1);

				while (1)
				{
					task_base ** originOld = origin.load();
					bool isAlreadyAllocOld = isAlreadyAlloc.load();

					if ((idOld & DEQUE_CNT) == 0 &&
						isAlreadyAllocOld == false &&
						isAlreadyAlloc.compare_exchange_weak(isAlreadyAllocOld, true))
					{
						auto arr = new std::array<task_base *, ARRAY_SIZE>;
						getArrayPool().push(arr);

						do
						{
							originOld = origin.load();
						} while (!origin.compare_exchange_weak(originOld, arr->data()));

						fixedDeque->ptr = arr->data() + (DEQUE_SIZE * (idOld & DEQUE_CNT));

						break;
					}
					else if ((idOld & DEQUE_CNT) != 0)
					{
						fixedDeque->ptr = originOld + (DEQUE_SIZE * (idOld & DEQUE_CNT));
						
						break;
					}

					continue;
				}
			}
        };

        struct controller
        {
            virtual ~controller() = default;

            crt_queue<fixed_task_deque *> dequeList;
        };

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
