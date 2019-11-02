#pragma once

#include <initializer_list>
#include <tuple>
#include <array>

#include "context_creator.h"
#include "util.h"
#include "task_container.h"

namespace task_executor
{
    /*
      backward push/pop, basically
      forward push/pop, without ownership
    */

    template<
        class T,
        class TaskDeque = task_deque,
        class TimedTaskMap = timed_task_map
    >
    struct executor_t :
        context_creator_t<executor_t<T>>
    {
        template<class Context>
        void release(std::initializer_list<Context*> contexts)
        {
            for (auto x : contexts)
                x->release(this);
        }

        template<class Task>
        void assign_front(Task* task)
        {
            getConcrete<T&>(*this).assign_front(task);
        }

        template<class Task>
        void assign_back(Task* task)
        {
            getConcrete<T&>(*this).assign_back(task);
        }

    private:
        TaskDeque* immediate;
        std::array<TimedTaskMap*, cntTimeSlot> shortTerm;
        std::array<TimedTaskMap*, cntTimeSlot> longTerm;
    };

    // Facade struct that can use executor through inheritance
}