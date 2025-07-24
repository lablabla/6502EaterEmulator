#pragma once

#include <cstdint>

namespace EaterEmulator::core
{
    using State = uint8_t;
    
    static constexpr uint8_t HIGH = 1;  // High state
    static constexpr uint8_t LOW = 0;   // Low state

    static constexpr uint8_t READ = HIGH;
    static constexpr uint8_t WRITE = LOW;
}