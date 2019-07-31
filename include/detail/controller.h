#pragma once

#include <atomic>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <deque>

namespace task_executor
{
    inline namespace controller_v1
    {
        struct controller
        {
            virtual ~controller() = default;
        };

        struct system :
            controller
        {
            system(const system &) = delete;
            system(system &&) = delete;
            system & operator= (const system &) = delete;
            system & operator= (system &&) = delete;

            system();
            ~system();

        protected:
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

            void removeThreads(const std::size_t currId)
            {
                auto reserves = getIdManager().getReserves(currId);

                for (auto& id : reserves)
                {
                    getIdManager().pushId(id);
                    getThreadManager().removeThread(id);
                }
            }

            void reserveRemoveSelf(const std::size_t id)
            {
                std::size_t cntRequires = getRequireManager().getReqCount();
                std::size_t cntReserves = getIdManager().getReserveCount();
                std::size_t cntThreads = getThreadManager().getCntThread();

                if (cntThreads - cntReserves > cntRequires)
                    getIdManager().reserveId(id);
            }

            const bool isStop(const std::size_t id)
            {
                return getThreadManager().getIsStop(id);
            }

            void stopAll()
            {
                std::thread{ [this]() { getThreadManager().removeAll(); } };
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

            public:
                static constexpr std::size_t INVALID_ID = 0;

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
                        threads[id] = new std::thread{ [this]() { thread_main(); } };

                        ++cntThreads;
                    }
                }

                void removeThread(const std::size_t id)
                {
                    stopThread(id);

                    std::lock_guard<std::mutex> lock{ mtx };

                    if (isStop.find(id) != isStop.end())
                    {
                        isStop.erase(id);

                        auto t = threads[id];
                        threads.erase(id);
                        delete t;

                        --cntThreads;
                    }
                }

                void removeAll()
                {
                    while (true)
                    {
                        std::lock_guard<std::mutex> lock{ mtx };

                        if (cntThreads == 0)
                            break;

                        removeThread(isStop.begin()->first);
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
                void stopThread(const std::size_t id)
                {
                    std::lock_guard<std::mutex> lock{ mtx };

                    if (isStop.find(id) != isStop.end())
                    {
                        isStop[id] = true;

                        threads[id]->join();
                    }
                }

                void thread_main()
                {

                }

            private:
                std::mutex mtx;
                std::condition_variable cv;
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

        private:
            static std::mutex mtx;
            static std::condition_variable cv;
        };

        struct thread_pool :
            controller
        {
            thread_pool(std::size_t reqThread);
            ~thread_pool();
        };
    }
}
