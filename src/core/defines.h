#pragma once

#include <cstdint>
#include <concepts>

namespace EaterEmulator::core
{
    using State = uint8_t;
    
    static constexpr uint8_t HIGH = 1;  // High state
    static constexpr uint8_t LOW = 0;   // Low state

    static constexpr uint8_t READ = HIGH;
    static constexpr uint8_t WRITE = LOW;

    template<typename T>
    concept Peripheral = requires(T device, int portId, uint8_t data) 
    {
        { device.writeToPort(portId, data) } -> std::same_as<void>;
    };
}