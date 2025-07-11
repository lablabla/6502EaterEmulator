#pragma once

#include <cstdint>

namespace EaterEmulator::core
{
    using ClockState = uint8_t;
    
    static constexpr uint8_t HIGH = 1;  // High state
    static constexpr uint8_t LOW = 0;   // Low state
}