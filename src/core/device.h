#pragma once

#include "core/bus.h"

#include <memory>
#include <string>

namespace EaterEmulator::core
{
    class Device
    {
    public:
        Device(std::shared_ptr<Bus> bus, uint16_t offset = 0) : _bus(bus), _offset(offset) {}
        virtual ~Device() = default;

        Device(const Device&) = default;
        Device& operator=(const Device&) = default;

        Device(Device&&) = delete;
        Device& operator=(Device&&) = delete;


        virtual bool shouldHandleAddress([[maybe_unused]]const uint16_t& address) const { return false; } // Default implementation, can be overridden
        virtual std::string getName() const = 0;

    protected:
        std::shared_ptr<Bus> _bus;
        uint16_t _offset; // Offset for the device, used to calculate the address range it handles
    };
}