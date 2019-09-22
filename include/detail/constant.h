#pragma once

#include <cstddef>
#include <chrono>

namespace task_executor
{
    constexpr std::size_t cntTimeSlot = 100;
    constexpr std::chrono::steady_clock::duration sizeTimeSlot =
        std::chrono::milliseconds{ 20 };
    constexpr std::chrono::steady_clock::duration sizeTotalTimeSlot =
        sizeTimeSlot * cntTimeSlot;
}