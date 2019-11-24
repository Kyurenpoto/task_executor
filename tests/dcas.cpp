#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include "task_executor.h"

TEST_CASE("test_dcas")
{
    using namespace task_executor::lockfree_op;

    SUBCASE("convert_atomic_type")
    {
        std::atomic_size_t x{ 0 };
        std::atomic<dcas_status_t>& y = reinterpret_cast<std::atomic<dcas_status_t>&>(x);

        REQUIRE(y.load() == dcas_status_t::UNDECIDED);

        y.store(dcas_status_t::SUCCESSED);

        REQUIRE(x.load() == 1);
    }

    SUBCASE("dcas_thread_1")
    {
        atomic_ext a, b;

        std::array<cas_requirement, 2> requirements{
            cas_requirement{.atom = a, .expectedValue = 0, .newValue = 1 },
            cas_requirement{.atom = b, .expectedValue = 0, .newValue = 1 },
        };

        REQUIRE(dcas(requirements));
        REQUIRE(a.value.load() == 1);
        REQUIRE(a.status.load() == value_status_t::NORMAL);
        REQUIRE(b.value.load() == 1);
        REQUIRE(b.status.load() == value_status_t::NORMAL);
    }

    SUBCASE("dcas_thread_1_seperate")
    {
        atomic_ext a, b;

        std::array<cas_requirement, 2> requirements1{
            cas_requirement{.atom = a, .expectedValue = 0, .newValue = 1 },
            cas_requirement{.atom = b, .expectedValue = 0, .newValue = 0 },
        }, requirements2{
            cas_requirement{.atom = a, .expectedValue = 0, .newValue = 0 },
            cas_requirement{.atom = b, .expectedValue = 0, .newValue = 1 },
        };

        while (!dcas(requirements1))
            requirements1[1].newValue = requirements1[1].expectedValue;

        {
            std::thread t2{ [&requirements = requirements2]() {
                while (!dcas(requirements))
                    requirements[0].newValue = requirements[0].expectedValue;
            } };

            t2.join();
        }

        REQUIRE(a.value.load() == 1);
        REQUIRE(a.status.load() == value_status_t::NORMAL);
        REQUIRE(b.value.load() == 1);
        REQUIRE(b.status.load() == value_status_t::NORMAL);
    }

    SUBCASE("dcas_thread_2")
    {
        atomic_ext a, b;

        std::array<cas_requirement, 2> requirements1{
            cas_requirement{.atom = a, .expectedValue = 0, .newValue = 1 },
            cas_requirement{.atom = b, .expectedValue = 0, .newValue = 0 },
        }, requirements2{
            cas_requirement{.atom = a, .expectedValue = 0, .newValue = 0 },
            cas_requirement{.atom = b, .expectedValue = 0, .newValue = 1 },
        };

        {
            std::thread t1{ [&requirements = requirements1]() {
            while (!dcas(requirements))
                requirements[1].newValue = requirements[1].expectedValue;
            } }, t2{ [&requirements = requirements2]() {
            while (!dcas(requirements))
                requirements[0].newValue = requirements[0].expectedValue;
            } };

            t1.join();
            t2.join();
        }

        REQUIRE(a.value.load() == 1);
        REQUIRE(a.status.load() == value_status_t::NORMAL);
        REQUIRE(b.value.load() == 1);
        REQUIRE(b.status.load() == value_status_t::NORMAL);
    }
}