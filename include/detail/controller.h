#pragma once

#include <atomic>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <deque>

#include "crt/hazard_pointer.h"

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
            system(const system &) = delete;
            system(system &&) = delete;
            system & operator= (const system &) = delete;
            system & operator= (system &&) = delete;

            system() = default;
            ~system()
            {
                stopAll();
            }

            void stopAll()
            {
                getThreadManager().removeAll(idThread);
            }

        protected:
            friend thread_pool;

            void addRequire(const std::size_t reqCount)
            {
                getRequireManager().addReqCount(reqCount);
            }

            void removeRequire(const std::size_t reqCount)
            {
                getRequireManager().removeReqCount(reqCount);
            }

            void addThreads()
            {
                getThreadManager().addThreads(
                    getRequireManager().getReqCount(), getIdManager());
            }

            void removeThreads()
            {
                auto reserves = getIdManager().getReserves(idThread);

                for (auto& id : reserves)
                {
                    getIdManager().pushId(id);
                    getThreadManager().removeThread(id);
                }
            }

            void reserveRemoveSelf()
            {
                std::size_t cntRequires = getRequireManager().getReqCount();
                std::size_t cntReserves = getIdManager().getReserveCount();
                std::size_t cntThreads = getThreadManager().getCntThread();

                if (cntThreads - cntReserves > cntRequires)
                    getIdManager().reserveId(idThread);
            }

            const bool isStop()
            {
                return getThreadManager().getIsStop(idThread);
            }

        private:
            struct require_manager
            {
                void addReqCount(const std::size_t reqCount)
                {
                    std::lock_guard<std::mutex> lock{ mtx };

                    reqCounts.insert(reqCount);

                    cv.notify_one();
                }

                void removeReqCount(const std::size_t reqCount)
                {
                    std::unique_lock<std::mutex> lock{ mtx };

                    while (reqCounts.empty())
                        cv.wait(lock);

                    auto it = reqCounts.find(reqCount);
                    if (it != reqCounts.end())
                        reqCounts.erase(it);
                }

                const std::size_t getReqCount()
                {
                    std::lock_guard<std::mutex> lock{ mtx };

                    return reqCounts.empty() ?
                        0 :
                        *reqCounts.rbegin();
                }

            private:
                std::multiset<std::size_t> reqCounts;
                std::mutex mtx;
                std::condition_variable cv;
            };

            struct id_manager
            {
                void pushId(const std::size_t id)
                {
                    std::lock_guard<std::mutex> lock{ mtx };

                    ids.push_back(id);
                }

                const std::size_t popId(const std::size_t cntThreads)
                {
                    std::lock_guard<std::mutex> lock{ mtx };

                    if (ids.empty())
                        return cntThreads + 1;
                    else
                    {
                        std::size_t id = ids.front();
                        ids.pop_front();
                        
                        return id;
                    }
                }

                void reserveId(const std::size_t id)
                {
                    std::lock_guard<std::mutex> lock{ mtx };

                    reserves.push_back(id);
                }

                const std::deque<std::size_t> getReserves(std::size_t idCurr)
                {
                    std::lock_guard<std::mutex> lock{ mtx };

                    std::deque<std::size_t> tmp = reserves;

                    for (auto & id : reserves)
                        if (id == idCurr)
                            return std::deque<std::size_t>{};

                    reserves.clear();

                    return tmp;
                }

                const std::size_t getReserveCount()
                {
                    std::lock_guard<std::mutex> lock{ mtx };

                    return reserves.size();
                }

            private:
                std::mutex mtx;
                std::deque<std::size_t> ids, reserves;
            };

            struct thread_manager
            {
                void addThreads(const std::size_t reqCount, id_manager& idManager)
                {
                    std::lock_guard<std::mutex> lock{ mtx };

                    while (cntThreads < reqCount)
                    {
                        std::size_t id = idManager.popId(cntThreads);
                        
                        isStop[id] = false;
                        threads[id] = new std::thread{ [this, id]() { thread_main(id); } };

                        ++cntThreads;
                    }
                }

                void removeThread(const std::size_t id)
                {
                    std::lock_guard<std::mutex> lock{ mtx };

                    if (isStop.find(id) == isStop.end())
                        return;

                    isStop[id] = true;
                    threads[id]->join();

                    isStop.erase(id);

                    auto t = threads[id];
                    threads.erase(id);
                    delete t;

                    --cntThreads;
                }

                void removeAll(const std::size_t id)
                {
                    std::unique_lock<std::mutex> lock{ mtx };

                    threads.erase(id);

                    lock.unlock();

                    while (true)
                    {
                        lock.lock();

                        if (isStop.empty())
                            return;

                        std::size_t idTarget = isStop.begin()->first;

                        lock.unlock();

                        removeThread(idTarget);
                    }
                }

                const std::size_t getCntThread()
                {
                    std::lock_guard<std::mutex> lock{ mtx };

                    return cntThreads;
                }

                const bool getIsStop(const std::size_t id)
                {
                    std::lock_guard<std::mutex> lock{ mtx };

                    return isStop.find(id) == isStop.end() ? false : isStop[id];
                }

            private:
                void thread_main(const std::size_t id)
                {
                    idThread = id;
                }

            private:
                std::mutex mtx;
                std::unordered_map<std::size_t, std::thread*> threads;
                std::unordered_map<std::size_t, bool> isStop;
                std::size_t cntThreads;
            };

            static require_manager& getRequireManager()
            {
                static require_manager instance;

                return instance;
            }

            static id_manager& getIdManager()
            {
                static id_manager instance;

                return instance;
            }

            static thread_manager& getThreadManager()
            {
                static thread_manager instance;

                return instance;
            }

            inline static thread_local std::size_t idThread = 0;
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
            {
                s.addRequire(reqThread);
            }

            ~thread_pool()
            {
                s.removeRequire(reqThread);
            }

        private:
            system s;
            std::size_t reqThread;
        };
    }

    namespace controller_v2
    {
        struct system final :
            controller
        {
            system(const system&) = delete;
            system(system&&) = delete;
            system& operator= (const system&) = delete;
            system& operator= (system&&) = delete;

            system() = default;
            ~system()
            {
                stopAll();
            }

            void stopAll();

        protected:
            friend thread_pool;

            void addRequire(const std::size_t reqCount);
            void removeRequire(const std::size_t reqCount);
            void adjustThreadCount();
            bool isStop();
            void setStop();

        private:
            struct node_ptr
            {
                std::atomic_bool isStop = false;
                std::thread threadObj;
            };

            struct thread_list_ptr
            {
                node_ptr * node = nullptr;
                thread_list_ptr * next = nullptr;
            };

            struct thread_list
            {
                node_ptr * add();

                void release(node_ptr *);

                std::atomic<thread_list_ptr *> head = nullptr;
            };

            node_ptr * getNode();
        };
    }
}
