#pragma once

#include <cstddef>
#include <atomic>

namespace task_executor
{
    enum class value_status_t : size_t
    {
        NORMAL = 0,
        RDCSS = 1,
        DCAS = 2,
        UNDECIDED = 3,
    };

    enum class dcas_status_t : size_t
    {
        UNDECIDED = 0,
        SUCCESSED = 1,
        FAILED = 2
    };

    struct alignas(sizeof(size_t)) atomic_ext
    {
        alignas(sizeof(size_t)) std::atomic<size_t> value = 0;
        alignas(sizeof(size_t)) std::atomic<value_status_t> status =
            value_status_t::NORMAL;
    };

    struct atomic_captured
    {
        size_t value = 0;
        value_status_t status = value_status_t::NORMAL;
    };

    struct rdcss_descriptor
    {
        std::atomic<size_t>& atom1;
        size_t expected1;
        atomic_ext& atom2;
        size_t expected2;
        size_t new2;
    };

    struct dcas_descriptor
    {
        std::atomic<dcas_status_t> status;
        atomic_ext& atom1;
        size_t expected1;
        size_t new1;
        atomic_ext& atom2;
        size_t expected2;
        size_t new2;
    };

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
                atom.value.compare_exchange_weak(oldValue, newValue);
                atom.status.store(newStatus);

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

    bool isRDCSSDescriptor(const atomic_captured& captured)
    {
        return captured.status == value_status_t::RDCSS;
    }

    bool isDCASDescriptor(const atomic_captured& captured)
    {
        return captured.status == value_status_t::DCAS;
    }

    void complete(rdcss_descriptor& descriptor)
    {
        size_t assignValue =
            (descriptor.atom1.load() == descriptor.expected1 ?
            descriptor.new2 : descriptor.expected2);

        cas1(descriptor.atom2,
            reinterpret_cast<size_t>(&descriptor), value_status_t::RDCSS,
            assignValue,
            value_status_t::NORMAL);
    }

    atomic_captured rdcss(rdcss_descriptor& descriptor)
    {
        atomic_captured captured;
        do
        {
            captured = cas1(descriptor.atom2,
                descriptor.expected2, value_status_t::NORMAL,
                reinterpret_cast<size_t>(&descriptor), value_status_t::RDCSS);
            if (isRDCSSDescriptor(captured))
                complete(*reinterpret_cast<rdcss_descriptor*>(captured.value));
        } while (isRDCSSDescriptor(captured));

        if (captured.value == descriptor.expected2)
            complete(descriptor);

        return captured;
    }

    bool dcas(dcas_descriptor& descriptor);

    void installDescriptor(dcas_descriptor& descriptor,
        atomic_ext& atom, size_t expected,
        dcas_status_t& status)
    {
        while (true)
        {
            rdcss_descriptor tmp{
                .atom1 = reinterpret_cast<std::atomic<size_t>&>(descriptor.status),
                .expected1 = static_cast<size_t>(dcas_status_t::UNDECIDED),
                .atom2 = atom,
                .expected2 = expected,
                .new2 = reinterpret_cast<size_t>(&descriptor)
            };
            atomic_captured captured = rdcss(tmp);

            if (!isDCASDescriptor(captured) ||
                captured.value == reinterpret_cast<size_t>(&descriptor))
                break;

            if (isDCASDescriptor(captured))
            {
                if (captured.value != reinterpret_cast<size_t>(&descriptor))
                {
                    dcas(reinterpret_cast<dcas_descriptor&>(captured.value));

                    continue;
                }
            }
            else if (captured.value != expected)
                status = dcas_status_t::FAILED;
        }
    }

    bool dcas(dcas_descriptor& descriptor)
    {
        dcas_status_t oldStatus = descriptor.status.load();
        if (oldStatus == dcas_status_t::UNDECIDED)
        {
            dcas_status_t status = dcas_status_t::SUCCESSED;

            installDescriptor(descriptor,
                descriptor.atom1, descriptor.expected1,
                status);

            if (status == dcas_status_t::SUCCESSED)
            {
                installDescriptor(descriptor,
                    descriptor.atom2, descriptor.expected2,
                    status);
            }

            descriptor.status.compare_exchange_weak(oldStatus, status);
        }

        bool successed =
            (descriptor.status.load() == dcas_status_t::SUCCESSED);

        cas1(descriptor.atom1,
            reinterpret_cast<size_t>(&descriptor), value_status_t::DCAS,
            (successed ? descriptor.new1 : descriptor.expected1),
            value_status_t::NORMAL);

        cas1(descriptor.atom2,
            reinterpret_cast<size_t>(&descriptor), value_status_t::DCAS,
            (successed ? descriptor.new2 : descriptor.expected2),
            value_status_t::NORMAL);

        return successed;
    }

    atomic_captured rdcssRead(atomic_ext& atom)
    {
        atomic_captured captured;
        do
        {
            captured = cas1(atom,
                0, value_status_t::NORMAL,
                0, value_status_t::NORMAL);
            if (isRDCSSDescriptor(captured))
                complete(*reinterpret_cast<rdcss_descriptor*>(captured.value));
        } while (isRDCSSDescriptor(captured));

        return captured;
    }

    atomic_captured dcasRead(atomic_ext& atom)
    {
        atomic_captured captured;
        do
        {
            captured = rdcssRead(atom);
            if (isDCASDescriptor(captured))
                dcas(*reinterpret_cast<dcas_descriptor*>(captured.value));
        } while (isDCASDescriptor(captured));

        return captured;
    }
}
