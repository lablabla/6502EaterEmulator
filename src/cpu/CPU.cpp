#include "cpu/CPU.h"
#include "cpu/Opcodes.h"

#include "devices/BUS.h"
#include "devices/Device.h"

#include "spdlog/spdlog.h"

#include <cstdint>
#include <vector>

namespace EaterEmulator 
{
    static constexpr uint16_t STACK_BASE_ADDRESS = 0x0100; // Base address for the stack
    static constexpr uint16_t RESET_VECTOR_ADDRESS = 0xFFFC; // Address for the reset vector

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
        registers.pc = fetchAddress(RESET_VECTOR_ADDRESS); // Fetch the reset vector from the ROM
    }

    void CPU::setDevices(const std::vector<std::shared_ptr<Device>>& devices)
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
            spdlog::warn("Unknown opcode: 0x{:02X}", static_cast<uint8_t>(opcode));
            return; // Handle unknown opcode gracefully
        }
        auto& opcodeInfo = OpcodeMap.at(opcode); // Get the opcode information
        std::vector<uint8_t> operands; // Vector to hold operands for the opcode
        decodeAddressingMode(opcodeInfo.mode, operands); // Decode the addressing mode and fetch operands
        uint16_t address = 0;
        uint8_t lowByte = 0;
        uint8_t highByte = 0;
        switch (opcode) {
            case Opcode::LDA_IMM: // Load Accumulator Immediate
                registers.a = operands[0]; // Load the immediate value into the accumulator
                updateStatusFlag(StatusBits::ZeroFlag, registers.a == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.a & 0x80);
                break;
            case Opcode::LDA_ZP: // Load Accumulator Zero Page
                address = operands[0]; // Get the zero page address
                registers.a = fetchByte(address); // Fetch the byte from the zero page address
                updateStatusFlag(StatusBits::ZeroFlag, registers.a == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.a & 0x80);
                break;
            case Opcode::LDA_ZPX: // Load Accumulator Zero Page,X
                address = (operands[0] + registers.x) & 0xFF; // Get the zero page address with X offset
                registers.a = fetchByte(address); // Fetch the byte from the zero page address
                updateStatusFlag(StatusBits::ZeroFlag, registers.a == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.a & 0x80);
                break;
            
            case Opcode::LDX_IMM: // Load X Register Immediate
                registers.x = operands[0]; // Load the immediate value into the X register
                updateStatusFlag(StatusBits::ZeroFlag, registers.x == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.x & 0x80);
                break;
            
            case Opcode::STA_ABS:
                address = makeUint16LE(operands[0], operands[1]);
                writeByte(address, registers.a);
                break;
            
            // Register Transfers
            case Opcode::TAX:
                registers.x = registers.a; // Transfer accumulator to X register
                updateStatusFlag(StatusBits::ZeroFlag, registers.x == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.x & 0x80);
                break;
            case Opcode::TXA:
                registers.a = registers.x; // Transfer X register to accumulator
                updateStatusFlag(StatusBits::ZeroFlag, registers.a == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.a & 0x80);
                break;
            case Opcode::DEX:
                registers.x--; // Decrement X register
                updateStatusFlag(StatusBits::ZeroFlag, registers.x == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.x & 0x80);
                break;
            case Opcode::INX:
                registers.x++; // Increment X register
                updateStatusFlag(StatusBits::ZeroFlag, registers.x == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.x & 0x80);
                break;
            case Opcode::TAY:
                registers.y = registers.a; // Transfer accumulator to Y register
                updateStatusFlag(StatusBits::ZeroFlag, registers.y == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.y & 0x80);
                break;
            case Opcode::TYA:
                registers.a = registers.y; // Transfer Y register to accumulator
                updateStatusFlag(StatusBits::ZeroFlag, registers.a == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.a & 0x80);
                break;
            case Opcode::DEY:
                registers.y--; // Decrement Y register
                updateStatusFlag(StatusBits::ZeroFlag, registers.y == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.y & 0x80);
                break;
            case Opcode::INY:
                registers.y++; // Increment Y register  
                updateStatusFlag(StatusBits::ZeroFlag, registers.y == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.y & 0x80);
                break;

            // Stack Operations
            case Opcode::TSX:
                registers.x = registers.sp; // Transfer stack pointer to X register
                updateStatusFlag(StatusBits::ZeroFlag, registers.x == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.x & 0x80);
                break;
            case Opcode::TXS:
                registers.sp = registers.x; // Transfer X register to stack pointer
                break;
            case Opcode::PHA:
                writeByte(STACK_BASE_ADDRESS + registers.sp, registers.a); // Push accumulator onto the stack
                registers.sp--; // Decrement stack pointer
                break;
            case Opcode::PLA:
                registers.sp++; // Increment stack pointer
                registers.a = fetchByte(STACK_BASE_ADDRESS + registers.sp); // Pull accumulator from the stack
                updateStatusFlag(StatusBits::ZeroFlag, registers.a == 0);
                updateStatusFlag(StatusBits::NegativeFlag, registers.a & 0x80);
                break;
            case Opcode::PHP:
                writeByte(STACK_BASE_ADDRESS + registers.sp, registers.status); // Push status register onto the stack
                registers.sp--; // Decrement stack pointer
                break;
            case Opcode::PLP:
                registers.sp++; // Increment stack pointer
                registers.status = fetchByte(STACK_BASE_ADDRESS + registers.sp); // Pull status register
                break;

            // Jump & Calls
            case Opcode::JMP_ABS:
                address = makeUint16LE(operands[0], operands[1]);
                registers.pc = address;
                break;
            case Opcode::JMP_IND:
                address = fetchAddress(makeUint16LE(operands[0], operands[1])); // Fetch the address from the indirect location
                registers.pc = address; // Set the program counter to the fetched address
                break;
            case Opcode::JSR:
                address = makeUint16LE(operands[0], operands[1]); // Get the address to jump to
                address--; // Decrement address to point to the instruction after JSR
                writeByte(STACK_BASE_ADDRESS + registers.sp, (registers.pc >> 8) & 0xFF); // Push high byte of PC onto stack
                registers.sp--; // Decrement stack pointer
                writeByte(STACK_BASE_ADDRESS + registers.sp, registers.pc & 0xFF); // Push low byte of PC onto stack
                registers.sp--; // Decrement stack pointer
                registers.pc = address; 
                break;
            case Opcode::RTS:
                registers.sp++; // Increment stack pointer to pull the low byte of the return address
                lowByte = fetchByte(STACK_BASE_ADDRESS + registers.sp); // Pull low byte from the stack
                registers.sp++; // Increment stack pointer to pull the high byte
                highByte = fetchByte(STACK_BASE_ADDRESS + registers.sp); // Pull high byte from the stack
                registers.pc = makeUint16LE(lowByte, highByte); // Set the program counter to the return address
                break;
                
            // Branches
            default:
                spdlog::warn("Unhandled opcode: 0x{:02X}", static_cast<uint8_t>(opcode));
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