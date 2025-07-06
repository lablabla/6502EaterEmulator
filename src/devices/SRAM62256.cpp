
#include "devices/SRAM62256.h"
#include "devices/Device.h"
#include "spdlog/spdlog.h"

namespace EaterEmulator 
{
    SRAM62256::SRAM62256()
        : Device(0x0000)          
    {
        spdlog::info("SRAM62256 initialized.");
    }

    bool SRAM62256::shouldHandlePins(CPU::Pins& pins) 
    {
        // Decode address from the address bus and check if it falls within the range of this SRAM
        // SRAM is mapped to addresses 0x0000 to 0x7FFF so only if A14 and A15 are LOW
        return pins.addressBus.getPin(14) == PinValue::LOW && 
               pins.addressBus.getPin(15) == PinValue::LOW;
    }

    void SRAM62256::handlePins(CPU::Pins& pins) 
    {
        if (!shouldHandlePins(pins)) 
        {
            return; // If the device should not handle the pins, return early
        }
        if (pins.RWB == PinValue::LOW) // If the RWB pin is low, it's a read operation
        {
            uint16_t address = pins.addressBus.getAddress(); // Get the address from the address bus
            pins.dataBus.setData(memory[address - _offset]); // Set the data bus with the value from memory
            spdlog::debug("{} - Read value {} from address 0x{:04X}", getName(), memory[address - _offset], address);
        }
        else
        {
            uint16_t address = pins.addressBus.getAddress(); // Get the address from the address bus
            memory[address - _offset] = pins.dataBus.getData(); // Write the value from the data bus to memory
            spdlog::debug("{} - Wrote value {} to address 0x{:04X}", getName(), pins.dataBus.getData(), address);
        }
    }
} // namespace EaterEmulator