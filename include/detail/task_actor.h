#pragma once

#include "task.h"
#include "thread_pool_executor.h"
#include "strand_executor.h"
#include "system_executor.h"

namespace task_executor
{
    struct task_actor_t :
        task_t<task_actor_t>
    {
    protected:
        template<class T>
        friend void task_t<T>::post();
        template<class T>
        friend void task_t<T>::defer();
        template<class T>
        friend void task_t<T>::dispatch();


        void post()
        {
            std::visit([task = this](auto& x)
                {
                    using T = std::decay_t<decltype(x)>;

                    if constexpr (std::is_same_v<T, nullptr_t>)
                    {
                        static_assert(std::is_same_v<T, nullptr_t>);
                    }
                    else
                    {
                        x->assign(task);
                    }
                }, executor);
        }

        void defer()
        {
            // try to change owned deque
        }

        void dispatch()
        {
            // execute immediately
        }
    };
}
