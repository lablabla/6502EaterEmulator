#pragma once

#include "devices/Device.h"

#include <array>
#include <vector>

namespace EaterEmulator 
{
    // EEPROM 28C256 is a 32K x 8-bit EEPROM
    class EEPROM28C256 : public Device
    {
    public:
        EEPROM28C256(const std::vector<uint8_t>& rom);
        ~EEPROM28C256() = default; // Default destructor

        EEPROM28C256(const EEPROM28C256&) = default;
        EEPROM28C256& operator=(const EEPROM28C256&) = default;

        EEPROM28C256(EEPROM28C256&&) = default;
        EEPROM28C256& operator=(EEPROM28C256&&) = default;
        
        bool shouldHandlePins(CPU::Pins& pins) override; // Override the check method from Device
        void handlePins(CPU::Pins& pins) override; // Override the action method from Device
        std::string getName() const override { return "EEPROM28C256"; } // Override the name method from Device

    private:
        std::array<uint8_t, 0x8000> memory; // 32K x 8-bit memory
    };
} // namespace EaterEmulator