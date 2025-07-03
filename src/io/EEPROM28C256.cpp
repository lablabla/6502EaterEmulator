
#include "io/EEPROM28C256.h"
#include "io/IODevice.h"
#include "spdlog/spdlog.h"

namespace EaterEmulator 
{
    EEPROM28C256::EEPROM28C256(const std::vector<uint8_t>& rom)
        : IODevice(0x8000)          
    {
        if (rom.size() != 0x8000) 
        {
            throw std::runtime_error("ROM size must be exactly 32K (0x8000 bytes)");
        }
        // Initialize memory with the contents of the ROM
        std::copy(rom.begin(), rom.end(), memory.begin());
        spdlog::info("EEPROM28C256 initialized with ROM data.");
    }

    bool EEPROM28C256::shouldHandlePins(CPU::Pins& pins) 
    {
        // Decode address from the address bus and check if it falls within the range of this EEPROM
        // EEPROM is mapped to addresses 0x8000 to 0xFFFF so only if A15 is HIGH
        return pins.addressBus.getPin(15) == PinValue::HIGH;
    }

    void EEPROM28C256::handlePins(CPU::Pins& pins) 
    {
        if (!shouldHandlePins(pins)) 
        {
            return; // If the device should not handle the pins, return early
        }
        if (pins.RWB == PinValue::LOW) // If the RWB pin is low, it's a read operation
        {
            uint16_t address = pins.addressBus.getAddress(); // Get the address from the address bus
            pins.dataBus.setData(memory[address - _offset]); // Set the data bus with the value from memory
        }
        // EEPROM is read only, so we don't handle write operations
    }
} // namespace EaterEmulator