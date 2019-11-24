/*
 * Algorithms were written referring to this paper:
 * ��A Practical Multi-Word Compare-and-Swap Operation�� by Timothy L. Harris, Keir Fraser, and Ian A. Pratt
 */

#pragma once

#include <cstddef>
#include <atomic>

namespace task_executor
{
    namespace lockfree_op
    {
        enum class value_status_t : size_t
        {
            NORMAL,
            RDCSS,
            DCAS,
            UNDECIDED,
        };

        struct alignas(sizeof(size_t)) atomic_ext
        {
            std::atomic_size_t value = 0;
            std::atomic<value_status_t> status = value_status_t::NORMAL;
        };

        struct atomic_captured
        {
            size_t value = 0;
            value_status_t status = value_status_t::NORMAL;
        };

        struct rdcss_descriptor
        {
            std::atomic_size_t& atom1;
            size_t expected1;
            atomic_ext& atom2;
            size_t expected2;
            size_t new2;
            value_status_t status2;
        };

        namespace detail
        {
            template<class T>
            size_t toSizeT(const T* ptr)
            {
                return reinterpret_cast<size_t>(ptr);
            }

            rdcss_descriptor* toRdcssDescriptor(const size_t x)
            {
                return reinterpret_cast<rdcss_descriptor*>(x);
            }
        }

        atomic_captured cas1(atomic_ext& atom,
            const size_t expectedValue, const value_status_t expectedStatus,
            const size_t newValue, const value_status_t newStatus)
        {
            while (true)
            {
                value_status_t oldStatus = expectedStatus;
                if (atom.status.compare_exchange_weak(
                    oldStatus, value_status_t::UNDECIDED))
                {
                    size_t oldValue = expectedValue;
                    bool isSuccessed =
                        atom.value.compare_exchange_weak(oldValue, newValue);
                    atom.status.store(isSuccessed ? newStatus : oldStatus);

                    return atomic_captured
                    { .value = oldValue, .status = oldStatus };
                }
                else if (oldStatus != value_status_t::UNDECIDED)
                {
                    return atomic_captured
                    { .value = atom.value.load(), .status = oldStatus };
                }
            }
        }

        bool isRdcssDescriptor(const atomic_captured& captured)
        {
            return captured.status == value_status_t::RDCSS;
        }

        void complete(rdcss_descriptor& descriptor)
        {
            size_t assignValue =
                (descriptor.atom1.load() == descriptor.expected1 ?
                    descriptor.new2 : descriptor.expected2);

            cas1(descriptor.atom2,
                detail::toSizeT(&descriptor), value_status_t::RDCSS,
                assignValue,
                descriptor.status2);
        }

        atomic_captured rdcss(rdcss_descriptor& descriptor)
        {
            atomic_captured captured;
            do
            {
                captured = cas1(descriptor.atom2,
                    descriptor.expected2, value_status_t::NORMAL,
                    detail::toSizeT(&descriptor), value_status_t::RDCSS);
                if (isRdcssDescriptor(captured))
                    complete(*detail::toRdcssDescriptor(captured.value));
            } while (isRdcssDescriptor(captured));

            if (captured.value == descriptor.expected2)
                complete(descriptor);

            return captured;
        }
    }
}