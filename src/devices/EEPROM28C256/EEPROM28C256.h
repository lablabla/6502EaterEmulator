#pragma once

#include "core/bus_slave.h"

#include <array>
#include <vector>

namespace EaterEmulator::devices
{
    // EEPROM 28C256 is a 32K x 8-bit EEPROM
    class EEPROM28C256 : public core::BusSlave
    {
    public:
        EEPROM28C256(const std::vector<uint8_t>& rom, core::Bus& bus);
        virtual ~EEPROM28C256();

        EEPROM28C256(const EEPROM28C256&) = delete;
        EEPROM28C256& operator=(const EEPROM28C256&) = delete;

        EEPROM28C256(EEPROM28C256&&) = delete;
        EEPROM28C256& operator=(EEPROM28C256&&) = delete;
        
        void handleBusNotification(uint16_t address, uint8_t rwb) override;

        bool shouldHandleAddress(const uint16_t& address) const override;
        
        std::string getName() const override { return "EEPROM28C256"; }

#ifdef UNIT_TEST
        const std::array<uint8_t, 0x8000>& getMemory() const { return _memory; }
#endif 

    private:
        std::array<uint8_t, 0x8000> _memory; // 32K x 8-bit memory
    };
} // namespace EaterEmulator