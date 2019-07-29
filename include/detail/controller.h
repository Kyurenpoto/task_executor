#pragma once

#include <atomic>

namespace task_executor
{
    inline namespace controller_v1
    {
        template<class Alloc>
        struct controller
        {
        };

        template<class Alloc>
        struct system :
            controller<Alloc>
        {
            system(const system &) = delete;
            system(system &&) = delete;
            system & operator= (const system &) = delete;
            system & operator= (system &&) = delete;

            system();

        protected:
            void activeThreads(std::size_t nLeast)
            {
                system & instance = system::getInstance();
                
                size_t nPrev = instance.nThreads;
                while (nPrev < nLeast &&
                    !instance.nThreads.compare_exchange_weak(nPrev, nLeast));
            }

        private:
            static system & getInstance()
            {
                static system ctrl;
                
                return ctrl;
            }

        private:
            static std::atomic<std::size_t> nThreads;
        };

        template<class Alloc>
        struct thread_pool :
            controller<Alloc>
        {
            thread_pool(std::size_t nThread);
        };
    }
}
