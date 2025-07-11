#pragma once

#include "core/clocked_device.h"

namespace EaterEmulator::core
{   
    class BusMaster : public ClockedDevice
    {
    public:
        BusMaster(Bus& bus, uint16_t offset = 0) : ClockedDevice(bus, offset) {}
        virtual ~BusMaster() = default;

        // Non-copyable, movable
        BusMaster(const BusMaster&) = delete;
        BusMaster& operator=(const BusMaster&) = delete;
        BusMaster(Bus&&) = delete;
        BusMaster& operator=(BusMaster&&) = delete;

        virtual void notifyBus(uint8_t rwb) = 0;
    };
}