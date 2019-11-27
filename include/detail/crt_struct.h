#pragma once

#include <atomic>
#include <array>
#include <optional>

#include "util.h"
#include "dcas.h"

namespace task_executor
{
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

    /*
     * Refered to this paper:
     * ¡°DCAS-BasedConcurrentDeques¡± by O Agesen et al.
     */

    template<class T, size_t N>
    struct lock_free_fixed_deque
    {
        void pushBack(T* data)
        {
            while (push(back, 1, reinterpret_cast<size_t>(data)) != 0);
        }

        void pushFront(T* data)
        {
            while (push(front, N - 1, reinterpret_cast<size_t>(data)) != 0);
        }

        T* popBack()
        {
            return reinterpret_cast<T*>(pop(back, N - 1));
        }

        T* popFront()
        {
            return reinterpret_cast<T*>(pop(front, 1));
        }

    private:
        lockfree_op::atomic_ext front, back{ .value = 1 };
        std::array<lockfree_op::atomic_ext, N> arr;

        size_t getOldAtom(
            lockfree_op::atomic_ext& atom)
        {
            lockfree_op::atomic_captured captured;
            do
            {
                captured = lockfree_op::cas1(atom,
                    0, lockfree_op::value_status_t::NORMAL,
                    0, lockfree_op::value_status_t::NORMAL);
            } while (captured.status != lockfree_op::value_status_t::NORMAL);

            return captured.value;
        }

        size_t push(
            lockfree_op::atomic_ext& atom,
            size_t increment,
            size_t data)
        {
            while (true)
            {
                size_t oldAtom = getOldAtom(atom) % N;
                size_t newAtom = (oldAtom + increment) % N;
                size_t oldElement = getOldAtom(arr[oldAtom]);
                size_t saveAtom = oldAtom;
                std::array<lockfree_op::cas_requirement, 2> requirements{
                    lockfree_op::cas_requirement{
                    .atom = atom, .expectedValue = oldAtom,
                    .newValue = (oldElement == 0 ? newAtom : oldAtom) },
                    lockfree_op::cas_requirement{
                    .atom = arr[oldAtom], .expectedValue = oldElement,
                    .newValue = data }
                };

                if (lockfree_op::dcas(requirements))
                    return oldElement;
                else if (requirements[0].expectedValue == saveAtom)
                    return 1;
            }
        }

        size_t pop(
            lockfree_op::atomic_ext& atom,
            size_t increment)
        {
            while (true)
            {
                size_t oldAtom = getOldAtom(atom) % N;
                size_t newAtom = (oldAtom + increment) % N;
                size_t oldElement = getOldAtom(arr[newAtom]);
                size_t saveAtom = oldAtom;
                std::array<lockfree_op::cas_requirement, 2> requirements{
                    lockfree_op::cas_requirement{
                    .atom = atom, .expectedValue = oldAtom,
                    .newValue = (oldElement == 0 ? oldAtom : newAtom) },
                    lockfree_op::cas_requirement{
                    .atom = arr[newAtom], .expectedValue = oldElement,
                    .newValue = 0 }
                };

                if (lockfree_op::dcas(requirements))
                    return oldElement;
                else if (requirements[0].expectedValue == saveAtom &&
                    requirements[1].expectedValue == 0)
                    return 0;
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