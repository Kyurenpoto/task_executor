#pragma once

#include "hazard_pointer.h"
#include "utils.h"

namespace task_executor
{
    inline namespace crt_queue_v1
    {
        template<class T>
        struct crt_queue
        {            
            void push(T * data)
            {
                node_ptr * node = xnew<node_ptr>(data);
                node_ptr * oldBack = back.load();

                if (oldBack == nullptr && back.compare_exchange_weak(oldBack, node))
                {
                    front.store(node);

                    return;
                }

                auto hazardPointer = hazardPointerManager.alloc();

                for (;;)
                {
                    oldBack = back.load();
                    hazardPointer->node = oldBack;

                    if (oldBack != back.load())
                        continue;

                    node_ptr * next = oldBack->next.load();
                    if (next != nullptr)
                    {
                        back.compare_exchange_weak(oldBack, next);

                        continue;
                    }

                    if (oldBack->next.compare_exchange_weak(next, node))
                    {
                        back.compare_exchange_weak(oldBack, node);

                        hazardPointerManager.release(hazardPointer);

                        node_ptr * oldFront = front.load();
                        if (oldFront == nullptr)
                            front.compare_exchange_weak(oldFront, node);

                        break;
                    }
                }
            }
            
            T * pop()
            {
                auto hazardPointer = hazardPointerManager.alloc();

                for (;;)
                {
                    node_ptr * oldFront = front.load();
                    node_ptr* oldBack = back.load();

                    hazardPointer->node = oldFront;

                    if (oldFront != front.load())
                        continue;

                    if (oldFront == nullptr)
                    {
                        hazardPointerManager.release(hazardPointer);

                        return nullptr;
                    }

                    node_ptr * next = oldFront->next.load();
                    if (oldFront == oldBack)
                    {
                        back.compare_exchange_weak(oldFront, next);

                        continue;
                    }

                    T * data = oldFront->data;

                    if (front.compare_exchange_weak(oldFront, next))
                    {
                        hazardPointerManager.release(hazardPointer);
                        hazardPointerManager.retire(oldFront);

                        return data;
                    }
                }
            }

        private:
            struct node_ptr
            {
                node_ptr(T * data) :
                    data{ data }
                {}

                T * data = nullptr;
                std::atomic<node_ptr *> next = nullptr;
            };

            std::atomic<node_ptr *> front = nullptr;
            std::atomic<node_ptr *> back = nullptr;
            hazard_pointer_manager hazardPointerManager;
        };
    }
}
