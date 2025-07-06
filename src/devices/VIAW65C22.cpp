
#include "devices/VIAW65C22.h"
#include "devices/Device.h"
#include "spdlog/spdlog.h"

namespace EaterEmulator 
{
    VIAW65C22::VIAW65C22()
        : Device(0x6000)          
    {
        spdlog::info("VIAW65C22 initialized.");
    }

    bool VIAW65C22::shouldHandlePins(CPU::Pins& pins) 
    {
        // Decode address from the address bus and check if it falls within the range of this VIA
        // VIA is mapped to addresses 0x6000 to 0x6FFF so we check if A13 & A14 are HIGH and A15 is LOW
        return pins.addressBus.getPin(13) == PinValue::HIGH && 
               pins.addressBus.getPin(14) == PinValue::HIGH && 
               pins.addressBus.getPin(15) == PinValue::LOW;
    }

    void VIAW65C22::handlePins(CPU::Pins& pins) 
    {
        if (!shouldHandlePins(pins)) 
        {
            return; // If the device should not handle the pins, return early
        }
        uint8_t address = pins.addressBus.getAddress() - _offset; // Get the address from the address bus and adjust for offset
        auto& registerValue = getRegister(address); // Get the register based on the address

        uint8_t mask = 0xFF;
        if (address == 0x00) // Port B
        {
            mask = registers.ddrb; // Use DDRB to determine which bits are input/output
        }
        else if (address == 0x01) // Port A
        {
            mask = registers.ddra; // Use DDRA to determine which bits are input/output
        }

        if (pins.RWB == PinValue::LOW) // If the RWB pin is low, it's a read operation
        {
            uint8_t value = registerValue & mask;
            pins.dataBus.setData(value);            
        }
        else
        {
            uint8_t value = pins.dataBus.getData(); // Get the value from the data bus
            registerValue = value & mask;
        }
    }


    uint8_t& VIAW65C22::getRegister(uint8_t address) 
    {
        // Map the address to the corresponding register
        switch (address) 
        {
            case 0x00: return registers.portb;
            case 0x01: return registers.porta;
            case 0x02: return registers.ddrb;
            case 0x03: return registers.ddra;
            case 0x04: return registers.t1cl;
            case 0x05: return registers.t1ch;
            case 0x06: return registers.t1ll;
            case 0x07: return registers.t1lh;
            case 0x08: return registers.t2cl;
            case 0x09: return registers.t2ch;
            case 0x0A: return registers.sr;
            case 0x0B: return registers.acr;
            case 0x0C: return registers.pcr;
            case 0x0D: return registers.ifr;
            case 0x0E: return registers.ier;
            case 0x0F: return registers.porta; // Port A no handshake
            default: throw std::out_of_range("Invalid register address");
        }
    }
} // namespace EaterEmulator