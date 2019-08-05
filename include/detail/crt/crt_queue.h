#pragma once

#include <cassert>

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

    namespace crt_queue_v2
    {
        template<class T>
        struct crt_queue
        {
            bool isEmpty()
            {
                for (;;)
                {
                    node_ptr * oldFront = front.load();

                    if (oldFront == nullptr)
                        return true;

                    T * oldBegin = oldFront->begin.load();
                    T * oldEnd = oldFront->end.load();

                    if (oldFront != front.load())
                        continue;

                    return oldBegin == oldEnd;
                }
            }

            void push(T && data)
            {
                auto hazard1 = hazardPointerManager.alloc();
                auto hazard2 = hazardPointerManager.alloc();

                for (;;)
                {
                    node_ptr * oldBack = back.load();
                    hazard1->node = oldBack;

                    if (oldBack != back.load())
                        continue;

                    if (oldBack == nullptr || oldBack->end - oldBack->buf == NODE_SIZE)
                    {
                        node_ptr* node = xnew<node_ptr>(data, getMemoryResource());

                        if (back.compare_exchange_weak(oldBack, node))
                        {
                            if (oldBack != nullptr)
                                oldBack->next.store(node);

                            hazardPointerManager.release(hazard1);
                            hazardPointerManager.release(hazard2);

                            return;
                        }

                        xdelete(node);
                    }

                    if (oldBack != back.load())
                        continue;

                    T * oldEnd;
                    T * newEnd;

                    do
                    {
                        oldEnd = oldBack->end.load();
                        hazard2->node = oldEnd;
                        newEnd = oldEnd + sizeof(T);
                    } while (oldEnd - oldBack->buf < NODE_SIZE &&
                        !oldBack->end.compare_exchange_weak(oldEnd, newEnd));

                    if (oldEnd - oldBack->buf == NODE_SIZE)
                        continue;

                    *oldEnd = data;
                    
                    hazardPointerManager.release(hazard2);
                    hazardPointerManager.release(hazard1);
                }
            }

            T pop()
            {
                auto hazard1 = hazardPointerManager.alloc();
                auto hazard2 = hazardPointerManager.alloc();

                for (;;)
                {
                    node_ptr * oldFront = front.load();

                    hazard1->node = oldFront;

                    if (oldFront != front.load())
                        continue;

                    if (oldFront == nullptr)
                    {
                        hazardPointerManager.release(hazard1);
                        hazardPointerManager.release(hazard2);

                        assert(oldFront != nullptr);
                    }

                    T * oldBegin = oldFront->begin.load();
                    T * oldEnd = oldFront->end.load();

                    if (oldFront != front.load())
                        continue;

                    T * newBegin;

                    do
                    {
                        oldBegin = oldFront->begin.load();
                        hazard2->node = oldBegin;
                        T * newBegin = oldBegin + sizeof(T);
                    } while (oldBegin != oldEnd &&
                        !oldFront->begin.compare_exchange_weak(oldBegin, newBegin));

                    if (oldBegin == oldEnd)
                    {
                        if (oldBegin - oldBack->buf == NODE_SIZE)
                        {
                            front.compare_exchange_weak(oldFront, oldFront->next.load());
                            back.compare_exchange_weak(oldFront, oldFront->next.load());

                            hazard1->node = nullptr;
                            hazardPointerManager.retire(oldFront);

                            continue;
                        }

                        hazardPointerManager.release(hazard1);
                        hazardPointerManager.release(hazard2);

                        assert(oldBegin != oldEnd);
                    }

                    T data = *oldBegin;

                    hazardPointerManager.release(hazard2);
                    hazardPointerManager.erase(oldBegin);
                    hazardPointerManager.release(hazard1);

                    return data;
                }
            }

        private:
            static constexpr std::size_t MAX_NODE_SIZE = 16;
            static constexpr std::size_t MAX_ARRAY_SIZE =
                MAX_NODE_SIZE < sizeof(T) ? 1 : MAX_NODE_SIZE / sizeof(T);
            static constexpr std::size_t NODE_SIZE = MAX_ARRAY_SIZE * sizeof(T);

            struct node_ptr
            {
                node_ptr(T && data, std::pmr::memory_resource * resource) :
                    resource{ resource }
                {
                    std::copy(&data, &data + sizeof(T), begin);
                }

                ~node_ptr()
                {
                    for (T * p = begin; p != end; p += sizeof(T))
                        p->~T();
                }

                std::atomic<T *> begin = buf;
                std::atomic<T *> end = buf + sizeof(T);
                std::atomic<node_ptr *> next = nullptr;
                std::pmr::memory_resource * resource = nullptr;
                std::byte buf[NODE_SIZE] = { 0 };
            };

            std::atomic<node_ptr *> front;
            std::atomic<node_ptr *> back;
            hazard_pointer_manager hazardPointerManager;
        };
    }
}
