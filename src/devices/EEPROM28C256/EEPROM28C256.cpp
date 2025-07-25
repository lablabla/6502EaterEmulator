
#include "devices/EEPROM28C256/EEPROM28C256.h"

#include "core/bus.h"
#include "core/defines.h"
#include "spdlog/spdlog.h"
#include <sys/types.h>

namespace EaterEmulator::devices
{
    EEPROM28C256::EEPROM28C256(const std::vector<uint8_t>& rom, std::shared_ptr<core::Bus> bus) 
        : core::BusSlave(bus, 0x8000), _memory{}
    {
        if (rom.size() != 0x8000) 
        {
            throw std::runtime_error("ROM size must be exactly 32K (0x8000 bytes)");
        }
        // Initialize memory with the contents of the ROM
        std::copy(rom.begin(), rom.end(), _memory.begin());
        spdlog::debug("EEPROM28C256 initialized with ROM data.");
    }

    EEPROM28C256::~EEPROM28C256() 
    {
        spdlog::debug("EEPROM28C256 destroyed.");
    }

    void EEPROM28C256::handleBusNotification(uint16_t address, uint8_t rwb)
    {
        if (!shouldHandleAddress(address)) {
            spdlog::debug("EEPROM28C256: Address {:#04x} not handled by this device", address);
            return; // If the pins are not for this device, do nothing
        }
        if (rwb == core::HIGH)
        {
            // EEPROM Only handles when clock is HIGH
            _bus->setData(_memory[address - _offset]);
        }
    }

    bool EEPROM28C256::shouldHandleAddress(const uint16_t& address) const
    {
        // Decode address from the address bus and check if it falls within the range of this EEPROM
        // EEPROM is mapped to addresses 0x8000 to 0xFFFF so only if A15 is HIGH
        return (address & (1 << 15)) != 0; 
    }
} // namespace EaterEmulator