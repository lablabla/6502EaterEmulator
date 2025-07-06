#pragma once

#include "devices/Device.h"

#include <array>

namespace EaterEmulator 
{
    // SRAM 62256 is a 32K x 8-bit SRAM
    class SRAM62256 : public Device
    {
    public:
        SRAM62256();
        ~SRAM62256() = default; // Default destructor

        SRAM62256(const SRAM62256&) = default;
        SRAM62256& operator=(const SRAM62256&) = default;

        SRAM62256(SRAM62256&&) = default;
        SRAM62256& operator=(SRAM62256&&) = default;
        
        bool shouldHandlePins(CPU::Pins& pins) override; // Override the check method from Device
        void handlePins(CPU::Pins& pins) override; // Override the action method from Device
        std::string getName() const override { return "SRAM62256"; } // Override the name method from Device

    private:
        std::array<uint8_t, 0x8000> memory; // 32K x 8-bit memory
    };
} // namespace EaterEmulator