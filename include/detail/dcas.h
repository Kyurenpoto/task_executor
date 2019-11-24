/*
 * Algorithms were written referring to this paper:
 * ��A Practical Multi-Word Compare-and-Swap Operation�� by Timothy L. Harris, Keir Fraser, and Ian A. Pratt
 */

#pragma once

#include <array>

#include "rdcss.h"

namespace task_executor
{
    namespace lockfree_op
    {
        enum class dcas_status_t : size_t
        {
            UNDECIDED,
            SUCCESSED,
            FAILED
        };

        struct cas_requirement
        {
            atomic_ext& atom;
            size_t expectedValue;
            size_t newValue;
        };

        struct dcas_descriptor
        {
            std::atomic<dcas_status_t> status;
            std::array<cas_requirement, 2>& requirements;
            std::array<size_t, 2> order;
            std::atomic<size_t> cntRef;

            static dcas_descriptor create(
                std::array<cas_requirement, 2>& requirements);

            void doDcas();
        };

        namespace detail
        {
            dcas_descriptor* toDcasDescriptor(const size_t x)
            {
                return reinterpret_cast<dcas_descriptor*>(x);
            }
        }

        bool isDcasDescriptor(const atomic_captured& captured)
        {
            return captured.status == value_status_t::DCAS;
        }

        void complete(dcas_descriptor& descriptor)
        {
            bool successed =
                (descriptor.status.load() == dcas_status_t::SUCCESSED);

            for (auto i : descriptor.order)
            {
                cas_requirement& requirement = descriptor.requirements[i];
                atomic_captured old = cas1(requirement.atom,
                    detail::toSizeT(&descriptor), value_status_t::DCAS,
                    (successed ? requirement.newValue : requirement.expectedValue),
                    value_status_t::NORMAL);

                if (isRdcssDescriptor(old))
                    requirement.expectedValue =
                    detail::toRdcssDescriptor(old.value)->expected2;
                else if (isDcasDescriptor(old))
                {
                    dcas_descriptor* oldDescriptor =
                        detail::toDcasDescriptor(old.value);

                    for (auto& r : oldDescriptor->requirements)
                        if (&r.atom == &requirement.atom)
                            requirement.expectedValue = r.expectedValue;
                }
                else
                    requirement.expectedValue = old.value;
            }
        }

        rdcss_descriptor createProxy(
            dcas_descriptor& descriptor,
            cas_requirement& requirement)
        {
            return rdcss_descriptor{
                .atom1 = reinterpret_cast<std::atomic_size_t&>(descriptor.status),
                .expected1 = static_cast<size_t>(dcas_status_t::UNDECIDED),
                .atom2 = requirement.atom,
                .expected2 = requirement.expectedValue,
                .new2 = detail::toSizeT(&descriptor),
                .status2 = value_status_t::DCAS
            };
        }

        bool wasInstalledSuccessfully(
            rdcss_descriptor& proxy,
            atomic_captured& captured)
        {
            return !isDcasDescriptor(captured) && captured.value == proxy.expected2;
        }

        bool hasPredecessorBeenFound(
            rdcss_descriptor& proxy,
            atomic_captured& captured)
        {
            return isDcasDescriptor(captured) && captured.value != proxy.new2;
        }

        bool isRdcssFailed(rdcss_descriptor& proxy, atomic_captured& captured)
        {
            return !isDcasDescriptor(captured) && captured.value != proxy.expected2;
        }

        void progressDescriptor(dcas_descriptor*& descriptor, dcas_status_t& oldStatus)
        {
            dcas_status_t status = dcas_status_t::SUCCESSED;

            for (auto i : descriptor->order)
            {
                rdcss_descriptor proxy =
                    createProxy(*descriptor, descriptor->requirements[i]);
                atomic_captured captured = rdcss(proxy);

                if (wasInstalledSuccessfully(proxy, captured))
                    continue;

                if (hasPredecessorBeenFound(proxy, captured))
                {
                    descriptor = detail::toDcasDescriptor(captured.value);
                    descriptor->cntRef.fetch_add(1);

                    return;
                }
                else if (isRdcssFailed(proxy, captured))
                {
                    status = dcas_status_t::FAILED;

                    break;
                }
            }

            descriptor->status.compare_exchange_weak(oldStatus, status);
        }

        dcas_descriptor dcas_descriptor::create(std::array<cas_requirement, 2>& requirements)
        {
            std::array<size_t, 2> order =
                (detail::toSizeT(&requirements[0].atom) <
                    detail::toSizeT(&requirements[1].atom)) ?
                std::array<size_t, 2>{0, 1} : std::array<size_t, 2>{1, 0};

            return dcas_descriptor{
                .status = dcas_status_t::UNDECIDED,
                .requirements = requirements,
                .order = order,
                .cntRef = 0
            };
        }

        void dcas_descriptor::doDcas()
        {
            dcas_descriptor* descriptor = this;
            while (true)
            {
                dcas_status_t oldStatus = descriptor->status.load();
                if (oldStatus != dcas_status_t::UNDECIDED)
                {
                    complete(*descriptor);

                    if (descriptor == this)
                        break;

                    descriptor->cntRef.fetch_sub(1);
                    descriptor = this;

                    continue;
                }

                progressDescriptor(descriptor, oldStatus);
            }

            while (cntRef.load() != 0);
        }

        bool dcas(std::array<cas_requirement, 2>& requirements)
        {
            dcas_descriptor descriptor = dcas_descriptor::create(requirements);
            descriptor.doDcas();

            return (descriptor.status.load() == dcas_status_t::SUCCESSED);
        }
    }
}