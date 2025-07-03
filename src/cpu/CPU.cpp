#include "cpu/CPU.h"
#include "cpu/Opcodes.h"

#include "io/IODevice.h"

#include "spdlog/spdlog.h"

#include <cstdint>
#include <vector>

namespace EaterEmulator 
{
    CPU::CPU(DataBus& dataBus, AddressBus& addressBus) 
    {
        _pins.addressBus = addressBus; // Initialize the address bus
        _pins.dataBus = dataBus; // Initialize the data bus
        _pins.RWB = PinValue::LOW; // Initialize the Read/Write-Bar control
    }

    void CPU::runNextInstruction() 
    {
        auto opcode = static_cast<Opcode>(fetchByte(registers.pc++)); // Fetch the next opcode
        handleOpcode(opcode);
    }

    void CPU::reset() 
    {
        // Reset the CPU state
        registers.x = 0;
        registers.y = 0;
        registers.a = 0;
        registers.status = 0;
        registers.sp = 0xFD; // Stack pointer starts at 0xFD

        // Read the reset vector from ROM
        registers.pc = fetchAddress(0xFFFC); // Fetch the reset vector from the ROM
    }

    void CPU::setDevices(const std::vector<std::shared_ptr<IODevice>>& devices)
    {
        _devices = devices; // Set the devices for the CPU
        spdlog::info("Devices set for CPU.");
    }

    void CPU::setAddressBus(uint16_t address)
    {
        for (int i = 0; i < 16; ++i) 
        {
            _pins.addressBus.setPin(i, (address >> i) & 1 ? PinValue::HIGH : PinValue::LOW); // Set each pin of the address bus
        }
    }

    void CPU::setDataBus(uint8_t data)
    {
        for (int i = 0; i < 8; ++i) 
        {
            _pins.dataBus.setPin(i, (data >> i) & 1 ? PinValue::HIGH : PinValue::LOW); // Set each pin of the data bus
        }        
    }

    void CPU::getDataBus(uint8_t& data)
    {
        data = 0; // Initialize data to 0
        for (int i = 0; i < 8; ++i) 
        {
            if (_pins.dataBus.getPin(i) == PinValue::HIGH) 
            {
                data |= (1 << i); // Set the corresponding bit in data if the pin is HIGH
            }
        }
    }

    void CPU::notifyDevices()
    {
        for (auto& device : _devices) 
        {
            device->handlePins(_pins); // Notify the device to handle the pins         
        }
        spdlog::debug("Devices notified with current pin state.");
    }

    uint16_t CPU::fetchAddress(uint16_t offset)
    {
        uint8_t lowByte = fetchByte(offset); // Fetch the low byte
        uint8_t highByte = fetchByte(offset+1); // Fetch the high byte
        uint16_t address = (highByte << 8) | lowByte; 
        return address; 
    }
    
    uint8_t CPU::fetchByte(uint16_t address)
    {
        _pins.RWB = PinValue::LOW; // Set the RWB pin to LOW for read operation
        setAddressBus(address); // Set the address bus to the specified address
        notifyDevices();
        // Data is now on the data bus
        uint8_t data = _pins.dataBus.getData();
        spdlog::debug("Fetched value {} from address 0x{:04X}", data, address);
        return data;
    }

    void CPU::writeByte(uint16_t address, uint8_t value)
    {
        _pins.RWB = PinValue::HIGH; // Set the RWB pin to HIGH for write operation
        setAddressBus(address); // Set the address bus to the specified address
        setDataBus(value); // Set the data bus with the value to write
        notifyDevices(); // Notify devices of the change
        spdlog::debug("Wrote value {} to address 0x{:04X}", value, address);
    }

    void CPU::handleOpcode(Opcode opcode)
    {
        switch (opcode) {
            default:
                spdlog::warn("Unhandled opcode: {}", static_cast<int>(opcode));
                break;
        }
    }

} // namespace EaterEmulator