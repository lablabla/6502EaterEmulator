
#include "devices/W65C02S/W65C02S.h"
#include "core/clocked_device.h"
#include "devices/W65C02S/opcodes.h"
#include "spdlog/spdlog.h"

#include <cstdint>
#include <iostream>

namespace EaterEmulator::devices
{
    W65C02S::W65C02S(core::Bus& bus)
        : core::BusMaster(bus)        
    {
        // Constructor implementation
    }

    W65C02S::~W65C02S()
    {
        // Destructor implementation
    }

    void W65C02S::reset()
    {
        // Initialize registers
        _a = 0; // Accumulator
        _x = 0; // X Register
        _y = 0; // Y Register
        _sp = 0xFF; // Stack Pointer
        _pc = 0xFFFC; // Program Counter - Reset vector
        _status = 0; // Processor Status
        _adl = 0; // Address Low Byte
        _adh = 0; // Address High Byte
    }

    void W65C02S::handleClockStateChange(core::ClockState state)
    {
        if (state == core::HIGH) 
        {
            handlePhi2High();
        } else {            
            handlePhi2Low();
        }
    }

    void W65C02S::notifyBus(uint8_t rwb)
    {
        _bus.notifySlaves(rwb);
    }

    void W65C02S::handlePhi2Low()
    {
        if (_stage < 2)
        {
            _bus.setAddress(_pc);
            _started = true; // Make sure we start the CPU on the PHI2 low
        }
        else
        {
            // Based on IR, we need to get the addressing mode and handle            
            auto it = OpcodeMap.find(_ir);
            if (it == OpcodeMap.end()) {
                std::cerr << "Unknown opcode: " << static_cast<int>(_ir) << std::endl;
                return;
            }
            switch(it->second.mode) 
            {
                case AddressingMode::IMM:
                    _bus.setAddress(_pc);
                    break;
                case AddressingMode::ZP: // Zero Page
                    _bus.setAddress(_adl);
                    break;
                case AddressingMode::ABS: // Absolute
                {
                    if (_stage == 2)
                    {
                        _bus.setAddress(_pc); // Read high byte of address
                    }
                    else
                    {
                        _bus.setAddress((_adh << 8) | _adl);
                    }
                }
                default:
                    break;
            }
        }
    }

    void W65C02S::handlePhi2High()
    {
        if (!_started)
        {
            return;
        }
        uint8_t rwb = _stage == 0 ? core::HIGH : getRWB();
        if (rwb == core::HIGH)
        {
            // Notify slaves that we are reading, set the data on the bus
            _bus.notifySlaves(rwb);
        }
        if (_stage == 0)
        {
            uint8_t opcode = fetchByte();
            _ir = static_cast<Opcode>(opcode); // Read the instruction from the data bus
            _pc++; // Increment the program counter
            _stage++; // Move to the next stage
            return;
        }
        auto it = OpcodeMap.find(_ir);
        if (it == OpcodeMap.end()) {
            std::cerr << "Unknown opcode: " << static_cast<int>(_ir) << std::endl;
            return; // Handle unknown opcode gracefully
        }
        const OpcodeInfo& opcodeInfo = it->second;
        if (_stage == opcodeInfo.cycles - 1) // If last cycle, we finished fetching, execute the instruction
        {
            bool incrementPC = true;
            executeInstruction(opcodeInfo.opcode, incrementPC);
            if (rwb == core::LOW)
            {
                // Notify the slaves that we are writing, take values from bus
                _bus.notifySlaves(rwb);
            }
            _stage = 0; // Reset stage for next instruction
            if (incrementPC)
            {
                _pc++;
            }
            return;
        }
        else if (_stage == 1)
        {
            _adl = fetchByte(); // Fetch the low byte of the address
            spdlog::debug("CPU: Read address low byte: {:#04x}", _adl);
            _stage++;
        }
        else if (_stage == 2)
        {
            _adh = fetchByte(); // Fetch the high byte of the address
            spdlog::debug("CPU: Read address high byte: {:#04x}", _adh);
            _stage++;
        }
        else 
        {
            spdlog::debug("CPU: unhandled stage {} for opcode: {:#04x}", _stage, static_cast<int>(_ir));
        }
        _pc++;
    }

    uint8_t W65C02S::fetchByte()
    {
        uint8_t data;
        _bus.getData(data); // Get data from the bus
        return data;
    }

    void W65C02S::executeInstruction(Opcode opcode, bool& incrementPC)
    {   
        incrementPC = true;
        switch(opcode)
        {
            case Opcode::LDA_IMM:
                _a = fetchByte();
                updateStatusFlags(_a);
                spdlog::debug("CPU: LDA_IMM executed, A = {:#04x}", static_cast<int>(_a));
                break;
            case Opcode::LDA_ZP:
                updateStatusFlags(_a);
                break;
            case Opcode::LDA_ABS:
                _a = fetchByte();
                updateStatusFlags(_a);
                spdlog::debug("CPU: LDA_ABS executed, A = {:#04x}", static_cast<int>(_a));
                break;
            case Opcode::STA_ABS:
                _bus.setData(_a); // Write the accumulator to the bus
                spdlog::debug("CPU: STA_ABS executed, A = {:#04x}", static_cast<int>(_a));
                break;

            case Opcode::JMP_ABS:
                _adh = fetchByte();
                spdlog::debug("CPU: Read address high byte: {:#04x}", _adh);
                _pc = (_adh << 8) | _adl;
                incrementPC = false; // Do not increment PC after a jump
                spdlog::debug("CPU: JMP_ABS executed, PC = {:#04x}", static_cast<int>(_pc));
                break;
            default:
                spdlog::error("CPU: Unhandled opcode: {:#04x}", static_cast<int>(opcode));
                break;
        }
    }

    void W65C02S::updateStatusFlags(uint8_t value)
    {
        // Update the status flags based on the value
        if (value == 0) {
            _status |= STATUS_ZERO; // Set zero flag
        } else {
            _status &= ~STATUS_ZERO; // Clear zero flag
        }
        if (value & 0x80) {
            _status |= STATUS_NEGATIVE; // Set negative flag
        } else {
            _status &= ~STATUS_NEGATIVE; // Clear negative flag
        }
        spdlog::debug("CPU: Status flags updated: {:#04x}", static_cast<int>(_status));
    }

    uint8_t W65C02S::getRWB() const
    {       
        auto it = OpcodeMap.find(_ir);
        if (it == OpcodeMap.end()) {
            std::cerr << "Unknown opcode: " << static_cast<int>(_ir) << std::endl;
            return 0;
        }
        switch(it->second.mode)
        {
            case AddressingMode::IMM:
            case AddressingMode::ZP:
                return core::HIGH; // Read operation
            case AddressingMode::ABS:                
                return _stage < 3 ? core::HIGH : it->second.rwb;
            default:
                return core::LOW; // Write operation
        }
    }
}