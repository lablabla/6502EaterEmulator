#pragma once

#include "core/bus.h"
#include <string>

namespace EaterEmulator::core
{
    class Device
    {
    public:
        Device(Bus& bus, uint16_t offset) : _bus(bus), _offset(offset) {}
        virtual ~Device() = default;

        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

        Device(Device&&) = delete;
        Device& operator=(Device&&) = delete;


        virtual bool shouldHandleAddress([[maybe_unused]]const uint16_t& address) const { return false; } // Default implementation, can be overridden
        virtual std::string getName() const = 0;

    protected:
        Bus& _bus;
        uint16_t _offset; // Offset for the device, used to calculate the address range it handles
    };
}