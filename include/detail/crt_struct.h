#pragma once

#include <atomic>
#include <array>
#include <optional>

#include "util.h"

namespace task_executor
{
    enum class atomic_status : size_t
    {
        NORMAL,
        UNDECIDED
    };

    template<class T>
    struct atomic_handle;

    template<>
    struct atomic_handle<size_t>
    {
        std::atomic_size_t value = 0;
        std::atomic<atomic_status> status = atomic_status::NORMAL;
    };

    template<class T>
    struct atomic_handle<T*>
    {
        std::atomic<T*> value = nullptr;
        std::atomic<atomic_status> status = atomic_status::NORMAL;
    };

    template<class T>
    struct lock_free_list
    {
        struct node
        {
            std::atomic<node*> next = nullptr;
            std::atomic<node*> prev = nullptr;
        };

        void pushBack(T data)
        {
            
        }

        void pushFront(T data)
        {

        }

        std::optional<T> popBack()
        {
            return std::nullopt;
        }

        std::optional<T> popFront()
        {
            return std::nullopt;
        }
    };

    template<class T, size_t N>
    struct lock_free_fixed_deque
    {
        void pushBack(T* data)
        {
            push(back, 1, data);
        }

        void pushFront(T* data)
        {
            push(front, N - 1, data);
        }

        T* popBack()
        {
            return pop(back, N - 1);
        }

        T* popFront()
        {
            return pop(front, 1);
        }

    private:
        atomic_handle<size_t> front, back{ .value = 1 };
        std::array<atomic_handle<T*>, N> arr;

        void push(atomic_handle<size_t>& atom, size_t increment, T* data)
        {
            while (true)
            {
                atomic_status oldAtomStatus = atomic_status::NORMAL;
                if (!atom.status.compare_exchange_weak(
                    oldAtomStatus, atomic_status::UNDECIDED))
                    continue;

                size_t oldAtom = atom.value.load();
                size_t newAtom = (oldAtom + increment) % N;

                atomic_status oldElementStatus = atomic_status::NORMAL;
                if (!arr[oldAtom].status.compare_exchange_weak(
                    oldElementStatus, atomic_status::UNDECIDED))
                {
                    atom.status.store(atomic_status::NORMAL);

                    continue;
                }

                T* oldElement = nullptr;
                bool isNotFull = arr[oldAtom].value.compare_exchange_weak(
                    oldElement, data);

                if (isNotFull)
                    atom.value.store(newAtom);
                atom.status.store(atomic_status::NORMAL);

                arr[oldAtom].status.store(atomic_status::NORMAL);

                if (isNotFull)
                    break;
            }
        }

        T* pop(atomic_handle<size_t>& atom, size_t increment)
        {
            while (true)
            {
                atomic_status oldAtomStatus = atomic_status::NORMAL;
                if (!atom.status.compare_exchange_weak(
                    oldAtomStatus, atomic_status::UNDECIDED))
                    continue;

                size_t oldAtom = atom.value.load();
                size_t newAtom = (oldAtom + increment) % N;

                atomic_status oldElementStatus = atomic_status::NORMAL;
                if (!arr[newAtom].status.compare_exchange_weak(
                    oldElementStatus, atomic_status::UNDECIDED))
                {
                    atom.status.store(atomic_status::NORMAL);

                    continue;
                }

                T* oldElement = arr[newAtom].value.load();
                bool isNotEmpty = (oldElement != nullptr);

                if (isNotEmpty)
                    atom.value.store(newAtom);
                atom.status.store(atomic_status::NORMAL);

                if (isNotEmpty)
                    arr[newAtom].value.store(nullptr);
                arr[newAtom].status.store(atomic_status::NORMAL);

                return oldElement;
            }
        }
    };

    template<class T>
    using crt_list = lock_free_list<T>;

    template<class T, size_t N>
    using crt_fixed_deque = lock_free_fixed_deque<T, N>;

    template<class T>
    struct lock_free_list_deque
    {
        template<class U>
        void pushBack(U && data)
        {
        }

        template<class U> 
        void pushFront(U && data)
        {
        }

        std::optional<T> popBack()
        {
            return std::nullopt;
        }

        std::optional<T> popFront()
        {
            return std::nullopt;
        }
    };

    template<class T>
    using crt_list_deque = lock_free_list_deque<T>;

    template<class T>
    struct lock_free_set
    {

    };

    template<class T>
    using crt_set = lock_free_set<T>;

    template<class Key, class T>
    struct lock_free_map
    {

    };

    template<class Key, class T>
    using crt_map = lock_free_map<Key, T>;

    template<size_t N>
    struct crt_bitset
    {
        crt_bitset() = default;

        crt_bitset(const crt_bitset& other)
        {
            for (size_t i = 0; i < SIZE; ++i)
                arr[i].store(other.arr[i].load());
        }

        crt_bitset& operator = (const crt_bitset& other)
        {
            for (size_t i = 0; i < SIZE; ++i)
                arr[i].store(other.arr[i].load());

            return *this;
        }

        crt_bitset(uint32_t value) :
            arr{ (std::byte)value, }
        {}

        void set(const size_t index)
        {
            if (index >= CNT_BITS)
                throw std::out_of_range("task_executor::crt_bitset<N>::set");

            std::byte oldVal = arr[index / FULL_BIT].load();

            arr[index / FULL_BIT].store(
                (std::byte)((int)oldVal | (1 << (index % FULL_BIT))));
        }

        bool test(const size_t index) const
        {
            if (index >= CNT_BITS)
                throw std::out_of_range("task_executor::crt_bitset<N>::set");

            std::byte oldVal = arr[index / FULL_BIT].load();

            return ((int)oldVal & (1 << (index % FULL_BIT)));
        }

        bool all() const
        {
            size_t cntFullBits = CNT_BITS / FULL_BIT;
            for (size_t i = 0; i < cntFullBits; ++i)
                if ((int)arr[i].load() != 0xFF)
                    return false;

            size_t cntRestBits = CNT_BITS % FULL_BIT;
            if (cntRestBits == 0)
                return true;

            return (int)arr[cntFullBits].load() == (1 << cntRestBits) - 1;
        }

        template<size_t N>
        crt_bitset<N>& operator |= (const crt_bitset<N>& rhs) noexcept
        {
            for (size_t i = 0; i < crt_bitset<N>::SIZE; ++i)
                arr[i].store(arr[i].load() | rhs.arr[i].load());

            return *this;
        }

    private:
        static constexpr size_t CNT_BITS = N;
        static constexpr size_t FULL_BIT = 8;
        static constexpr size_t SIZE = (N + FULL_BIT - 1) / FULL_BIT;

        std::array<std::atomic<std::byte>, SIZE> arr{};
    };

    template<size_t N>
    crt_bitset<N> operator | (
        const crt_bitset<N>& lhs,
        const crt_bitset<N>& rhs)
        noexcept
    {
        crt_bitset<N> result;
        result = lhs;

        return (result |= rhs);
    }

    template<>
    struct crt_bitset<0>
    {};
}