#pragma once

#include "core/bus_slave.h"

#include <array>
#include <vector>

namespace EaterEmulator::devices
{
    // SRAM 62256 is a 32K x 8-bit SRAM
    class SRAM62256 : public core::BusSlave
    {
    public:
        SRAM62256(core::Bus& bus);
        virtual ~SRAM62256();

        SRAM62256(const SRAM62256&) = delete;
        SRAM62256& operator=(const SRAM62256&) = delete;

        SRAM62256(SRAM62256&&) = delete;
        SRAM62256& operator=(SRAM62256&&) = delete;
        
        void handleBusNotification(uint16_t address, uint8_t rwb) override;

        bool shouldHandleAddress(const uint16_t& address) const override;
        
        std::string getName() const override { return "SRAM62256"; }

#ifdef UNIT_TEST
        std::array<uint8_t, 0x8000>& getMemory() { return _memory; }
#endif 

    private:
        std::array<uint8_t, 0x8000> _memory; // 32K x 8-bit memory

    };
} // namespace EaterEmulator