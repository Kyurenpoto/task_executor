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

    template<class T>
    struct executor_t :
        context_creator_t<executor_t<T>>
    {
        template<class Context>
        void release(std::initializer_list<Context*> contexts)
        {
            for (auto x : contexts)
                x->release(this);
        }

        // push & execute
        template<class Task>
        void assign(Task* task)
        {
            getConcrete<T&>(*this).assign(task);
        }

    private:
        task_deque immediate;
        std::array<timed_task_map*, cntTimeSlot> shortTerm;
        std::array<timed_task_map*, cntTimeSlot> longTerm;
    };

    // Facade struct that can use executor through inheritance
    struct executable_t
    {

    };
}