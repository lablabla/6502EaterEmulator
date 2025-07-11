#pragma once

#include "core/device.h"

namespace EaterEmulator::core
{
    class BusSlave : public Device
    {
    public:
        BusSlave(Bus& bus, uint16_t offset = 0) : Device(bus, offset) {}
        virtual ~BusSlave() = default;

        // Non-copyable, movable
        BusSlave(const BusSlave&) = delete;
        BusSlave& operator=(const BusSlave&) = delete;
        BusSlave(Bus&&) = delete;
        BusSlave& operator=(BusSlave&&) = delete;

        virtual void handleBusNotification(uint16_t address, uint8_t rwb) = 0;

        void addBusSlave(BusSlave* slave)
        {
            _bus.addSlave(slave);
        }
    };
}