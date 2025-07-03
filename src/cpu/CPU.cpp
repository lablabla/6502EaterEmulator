#include "cpu/CPU.h"
#include "cpu/Opcodes.h"

#include "io/BUS.h"
#include "io/IODevice.h"

#include "spdlog/spdlog.h"

#include <cstdint>
#include <vector>

namespace EaterEmulator 
{
    static uint16_t makeUint16LE(uint8_t low, uint8_t high)
    {
        uint16_t address = (static_cast<uint16_t>(high) << 8) | low; 
        return address; 
    }
    
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
        return makeUint16LE(lowByte, highByte); 
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
        if (!OpcodeMap.contains(opcode)) 
        {
            spdlog::warn("Unknown opcode: {}", static_cast<int>(opcode));
            return; // Handle unknown opcode gracefully
        }
        auto& opcodeInfo = OpcodeMap.at(opcode); // Get the opcode information
        std::vector<uint8_t> operands; // Vector to hold operands for the opcode
        decodeAddressingMode(opcodeInfo.mode, operands); // Decode the addressing mode and fetch operands
        uint16_t address = 0;
        switch (opcode) {
            case Opcode::LDA_IMM: // Load Accumulator Immediate
                registers.a = operands[0]; // Load the immediate value into the accumulator
                updateStatusFlag(StatusBits::ZeroFlag, registers.a == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.a & 0x80);
                break;
            
            case Opcode::STA_ABS:
                address = makeUint16LE(operands[0], operands[1]);
                writeByte(address, registers.a);
                break;

            case Opcode::JMP_ABS:
                address = makeUint16LE(operands[0], operands[1]);
                registers.pc = address;
                break;
                
            default:
                spdlog::warn("Unhandled opcode: {}", static_cast<int>(opcode));
                break;
        }
    }

    void CPU::decodeAddressingMode(AddressingMode addressingMode, std::vector<uint8_t>& operands)
    {
        operands.clear(); // Clear the operands vector
        switch (addressingMode) {
            case AddressingMode::IMP: // Implied mode
            case AddressingMode::ACC: // Accumulator mode
                // No operands needed
                break;
            case AddressingMode::IMM: // Immediate mode
            case AddressingMode::REL: // Relative mode
            case AddressingMode::ZP: // Zero Page mode
            case AddressingMode::ZPX: // Zero Page,X mode
            case AddressingMode::ZPY: // Zero Page,Y mode
                operands.push_back(fetchByte(registers.pc++)); // Store the zero page address
                break;
            case AddressingMode::ABS: // Absolute mode
            case AddressingMode::ABSX: // Absolute,X mode
            case AddressingMode::ABSY: // Absolute,Y mode
            case AddressingMode::IND: // Indirect mode
            case AddressingMode::INDX: // Indirect,X mode
            case AddressingMode::INDY: // Indirect,Y mode
                operands.push_back(fetchByte(registers.pc++)); // Fetch the low byte
                operands.push_back(fetchByte(registers.pc++)); // Fetch the high byte
                break;
            default:
                spdlog::warn("Unhandled addressing mode: {}", static_cast<int>(addressingMode));
                break;
        }
    }

    void CPU::updateStatusFlag(StatusBits flag, bool value)
    {
        if (value)
        {
            setStatusFlag(flag);
        }
        else
        {
            clearStatusFlag(flag);
        }
    }

    void CPU::setStatusFlag(StatusBits flag)
    {
        registers.status |= static_cast<uint8_t>(flag); // Set the specified status flag
        spdlog::debug("Status flag {} set. Current status: 0x{:02X}", static_cast<int>(flag), registers.status);
    }

    void CPU::clearStatusFlag(StatusBits flag)
    {
        registers.status &= ~static_cast<uint8_t>(flag); // Clear the specified status flag
        spdlog::debug("Status flag {} cleared. Current status: 0x{:02X}", static_cast<int>(flag), registers.status);
    }


} // namespace EaterEmulator