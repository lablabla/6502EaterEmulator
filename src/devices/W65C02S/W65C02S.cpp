
#include "devices/W65C02S/W65C02S.h"
#include "core/defines.h"
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
        _resetStage = 0;
    }

    void W65C02S::handleClockStateChange(core::ClockState state)
    {
        if (state == core::HIGH) 
        {
            handlePhi2HighAddressing();
        } else {            
            handlePhi2LowAddressing();
        }
    }

    void W65C02S::notifyBus(uint8_t rwb)
    {
        _bus.notifySlaves(rwb);
    }


    void W65C02S::handlePhi2LowAddressing()
    {
         // Based on IR, we need to get the addressing mode and handle
        auto it = OpcodeMap.find(_ir);
        if (it == OpcodeMap.end()) {
            std::cerr << "Unknown opcode: " << static_cast<int>(_ir) << std::endl;
            return;
        }
        const auto opcodeInfo = it->second;
        const auto addressingMode = opcodeInfo.addressingMode;   
        if (_cycle == 0)
        {
            _bus.setAddress(_pc);
            _started = true; // Make sure we start the CPU on the PHI2 low
        }
        else
        {
            bool handled = false;
            switch (addressingMode)
            {
                case AddressingMode::IMM:
                    handled = handleImmediateAddressing(opcodeInfo, core::LOW);
                    break;

                case AddressingMode::ABS:
                    handled = handleAbsoluteAddressing(opcodeInfo, core::LOW);
                    break;
                case AddressingMode::ABSX:
                case AddressingMode::ABSY:
                    handled = handleAbsoluteIndexedAddressing(opcodeInfo, core::LOW);
                    break;

                case AddressingMode::ZP:
                    handled = handleZeroPageAddressing(opcodeInfo, core::LOW);
                    break;
                case AddressingMode::ZPX:
                case AddressingMode::ZPY:
                    handled = handleZeroPageIndexedAddressing(opcodeInfo, core::LOW);
                    break;
                default:
                    handled = false;
                    break;
            }
            if (!handled)
            {
                spdlog::error("Unhandled addressing opcode: {:#04x}", static_cast<int>(opcodeInfo.opcode));
            }
        }
    }

    void W65C02S::handlePhi2HighAddressing()
    {
        if (!_started)
        {
            return;
        }
        bool inReset = _resetStage < 2;
        if (inReset)
        {
            handleReset();
        }
        else if (_cycle == 0)
        {

            uint8_t opcode = fetchByte();
            _ir = static_cast<Opcode>(opcode); // Read the instruction from the data bus
            _pc++;
            _cycle++;
        }
        else
        {
            auto it = OpcodeMap.find(_ir);
            if (it == OpcodeMap.end()) {
                std::cerr << "Unknown opcode: " << static_cast<int>(_ir) << std::endl;
                return;
            }
            const auto opcodeInfo = it->second;
            const auto addressingMode = opcodeInfo.addressingMode;
            bool handled = false;
            switch (addressingMode)
            {
                case AddressingMode::IMM:
                    handled = handleImmediateAddressing(opcodeInfo, core::HIGH);
                    break;

                case AddressingMode::ABS:
                    handled = handleAbsoluteAddressing(opcodeInfo, core::HIGH);
                    break;
                case AddressingMode::ABSX:
                case AddressingMode::ABSY:
                    handled = handleAbsoluteIndexedAddressing(opcodeInfo, core::HIGH);
                    break;

                case AddressingMode::ZP:
                    handled = handleZeroPageAddressing(opcodeInfo, core::HIGH);
                    break;
                case AddressingMode::ZPX:
                case AddressingMode::ZPY:
                    handled = handleZeroPageIndexedAddressing(opcodeInfo, core::HIGH);
                    break;
                default:
                    handled = false;
                    break;
            }
            if (!handled)
            {
                spdlog::error("Unhandled addressing opcode: {:#04x}", static_cast<int>(opcodeInfo.opcode));
            }
            _cycle++;
            if(_cycle == opcodeInfo.cycles)
            {
                _cycle = 0;
            }
        }
    }

    void W65C02S::handleReset()
    {
        if (_resetStage == 0)
        {
            _adl = fetchByte(); // Read low byte of reset vector
            spdlog::debug("CPU: Read reset vector low byte: {:#04x}", _adl);
            _resetStage++;
            _pc++;
        }
        else
        {
            _adh = fetchByte(); // Read high byte of reset vector
            spdlog::debug("CPU: Read reset vector high byte: {:#04x}", _adh);
            _resetStage++;
            _pc = (_adh << 8) | _adl; // Set program counter to reset vector
            spdlog::debug("CPU: Reset complete, PC set to {:#04x}", _pc);
        }
    }


    bool W65C02S::handleImmediateAddressing(const OpcodeInfo& info, core::ClockState clockState)
    {
        if (clockState == core::LOW)
        {
            return handleImmediateLow(info);
        }
        return handleImmediateHigh(info);
    }   
    
    bool W65C02S::handleImmediateLow([[maybe_unused]]const OpcodeInfo& info)
    {
        if (_cycle == 1)
        {
            // fetch low byte of address, increment PC
            _bus.setAddress(_pc++);
            return true;
        }
        return false;        
    }
    
    bool W65C02S::handleImmediateHigh(const OpcodeInfo& info)
    {
        if (_cycle == 1)
        {
            switch(info.opcode)
            {
                case Opcode::LDA_IMM:
                    _a = fetchByte();
                    updateStatusFlags(_a);
                    break;
                case Opcode::LDX_IMM:
                    _x = fetchByte();
                    updateStatusFlags(_x);
                    break;
                case Opcode::LDY_IMM:
                    _y= fetchByte();
                    updateStatusFlags(_y);
                    break;
                default:
                    return false;
            }
            return true;
        }
        return false;
    }

    bool W65C02S::handleAbsoluteAddressing(const OpcodeInfo& info, core::ClockState clockState)
    {
        if (clockState == core::LOW)
        {
            return handleAbsoluteLow(info);
        }
        return handleAbsoluteHigh(info);
    }
    bool W65C02S::handleAbsoluteLow(const OpcodeInfo& info)
    {
        if (_cycle == 1)
        {
            // fetch low byte of address, increment PC
            _bus.setAddress(_pc++);
        }
        else if (_cycle == 2)
        {
            // fetch high byte of address, increment PC
            _bus.setAddress(_pc++);
        }
        else if (_cycle == 3)
        {
            // read from effective address
            _bus.setAddress((_adh << 8) | _adl);
        }
        else
        {
            spdlog::error("Unhandled cycle {} for ABS low clock, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
            return false;
        }
        return true;
    }
    bool W65C02S::handleAbsoluteHigh(const OpcodeInfo& info)
    {
        if (_cycle == 1)
        {
            // fetch low byte of address, increment PC
            _adl = fetchByte();
        }
        else if (_cycle == 2)
        {
            // fetch high byte of address, increment PC
            _adh = fetchByte();
        }
        else if (_cycle == 3)
        {
            switch (info.opcode)
            {
                // Read instructions
                case Opcode::LDA_ABS:
                case Opcode::LDA_ABSX:
                case Opcode::LDA_ABSY:
                    _a = fetchByte();
                    updateStatusFlags(_a);
                    break;
                case Opcode::LDX_ABS:
                case Opcode::LDX_ABSY:
                    _x = fetchByte();
                    updateStatusFlags(_x);
                    break;
                case Opcode::LDY_ABS:
                case Opcode::LDY_ABSX:
                    _y = fetchByte();
                    updateStatusFlags(_y);
                    break;

                // Read-modify-write instructions
                // Write instructions
                case Opcode::STA_ABS:
                case Opcode::STA_ABSX:
                case Opcode::STA_ABSY:
                    writeByte(_a);
                    break;
                case Opcode::STX_ABS:
                case Opcode::STX_ZPY:
                    writeByte(_x);
                    break;
                case Opcode::STY_ABS:
                case Opcode::STY_ZPX:
                    writeByte(_y);
                    break;
                default:                    
                    spdlog::error("Unhandled opcode for ABS low clock, opcode: {:#04x}", static_cast<int>(info.opcode));
                    return false;
            }
        }
        else
        {
            spdlog::error("Unhandled cycle {} for ABS low clock, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
            return false;
        }
        return true;
    }

    bool W65C02S::handleAbsoluteIndexedAddressing(const OpcodeInfo& info, core::ClockState clockState)
    {
        if (clockState == core::LOW)
        {
            return handleAbsoluteIndexedLow(info);
        }
        return handleAbsoluteIndexedHigh(info);
    }

    bool W65C02S::handleAbsoluteIndexedLow(const OpcodeInfo& info)
    {
        uint8_t& indexingRegister = info.addressingMode == AddressingMode::ABSX ? _x : _y;
        if (_cycle == 1)
        {
            // fetch low byte of address, increment PC
            _bus.setAddress(_pc++);
        }
        else if (_cycle == 2)
        {
            // fetch high byte of address, increment PC
            _bus.setAddress(_pc++);
        }
        else if (_cycle == 3)
        {
            // read from effective address
            _bus.setAddress(((_adh << 8) | _adl) + indexingRegister);
        }
        else
        {
            // TODO: Handle page crossing
            spdlog::error("Unhandled cycle {} for ABS Indexed low clock, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
            return false;
        }
        return true;
    }

    bool W65C02S::handleAbsoluteIndexedHigh(const OpcodeInfo& info)
    {
        return handleAbsoluteHigh(info); // Same as absolute high, just with indexing register added
    }


    bool W65C02S::handleZeroPageAddressing(const OpcodeInfo& info, core::ClockState clockState)
    {
        if (clockState == core::LOW)
        {
            return handleZeroPageLow(info);
        }
        return handleZeroPageHigh(info);
    }
    bool W65C02S::handleZeroPageLow(const OpcodeInfo& info)
    {
        if (_cycle == 1)
        {
            // fetch low byte of address, increment PC
            _bus.setAddress(_pc++);
        }
        else if (_cycle == 2)
        {
            // read from effective address
            _bus.setAddress( _adl);
        }
        else
        {
            spdlog::error("Unhandled cycle {} for ZP low clock, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
            return false;
        }
        return true;
    }
    bool W65C02S::handleZeroPageHigh(const OpcodeInfo& info)
    {
        if (_cycle == 1)
        {
            // fetch address, increment PC
            _adl = fetchByte();
        }
        else if (_cycle == 2)
        {
            switch (info.opcode)
            {
                // Read instructions
                case Opcode::LDA_ZP:
                case Opcode::LDA_ZPX:
                    _a = fetchByte();
                    updateStatusFlags(_a);
                    break;
                case Opcode::LDX_ZP:
                case Opcode::LDX_ZPY:
                    _x = fetchByte();
                    updateStatusFlags(_x);
                    break;
                case Opcode::LDY_ZP:
                case Opcode::LDY_ZPX:
                    _y = fetchByte();
                    updateStatusFlags(_y);
                    break;

                // Read-modify-write instructions
                // Write instructions
                case Opcode::STA_ZP:
                case Opcode::STA_ZPX:
                    writeByte(_a);
                    break;
                case Opcode::STX_ZP:
                case Opcode::STX_ZPY:
                    writeByte(_x);
                    break;
                case Opcode::STY_ZP:
                case Opcode::STY_ZPX:
                    writeByte(_y);
                    break;
                default:                    
                    spdlog::error("Unhandled opcode for ZP low clock, opcode: {:#04x}", static_cast<int>(info.opcode));
                    return false;
            }
        }
        else
        {
            spdlog::error("Unhandled cycle {} for ZP low clock, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
            return false;
        }
        return true;
    }

        bool W65C02S::handleZeroPageIndexedAddressing(const OpcodeInfo& info, core::ClockState clockState)
        {
        if (clockState == core::LOW)
        {
            return handleZeroPageIndexedLow(info);
        }
        return handleZeroPageIndexedHigh(info);
        }        
        bool W65C02S::handleZeroPageIndexedLow(const OpcodeInfo& info)
        {
            if (_cycle == 1)
            {
                // fetch low byte of address, increment PC
                _bus.setAddress(_pc++);
            }
            else if (_cycle == 2)
            {
                // read from effective address
                _bus.setAddress( _adl);
            }
            else if (_cycle == 3)
            {
                _bus.setAddress(_adh);
            }
            else
            {
                spdlog::error("Unhandled cycle {} for ZP indexed low clock, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
                return false;
            }
            return true;
        }
        bool W65C02S::handleZeroPageIndexedHigh(const OpcodeInfo& info)
        {
        if (_cycle == 1)
        {
            // fetch address, increment PC
            _adl = fetchByte();
        }
        else if (_cycle == 2)
        {
            // read from effective address
            _adh = _adl + (info.addressingMode == AddressingMode::ZPX ? _x : _y);
        }
        else if (_cycle == 3)
        {
            switch (info.opcode)
            {
                // Read instructions
                case Opcode::LDA_ZPX:
                    _a = fetchByte();
                    updateStatusFlags(_a);
                    break;
                case Opcode::LDX_ZPY:
                    _x = fetchByte();
                    updateStatusFlags(_x);
                    break;
                case Opcode::LDY_ZPX:
                    _y = fetchByte();
                    updateStatusFlags(_y);
                    break;

                // Read-modify-write instructions
                // Write instructions
                case Opcode::STA_ZPX:
                    writeByte(_a);
                    break;
                case Opcode::STX_ZPY:
                    writeByte(_x);
                    break;
                case Opcode::STY_ZPX:
                    writeByte(_y);
                    break;
                default:                    
                    spdlog::error("Unhandled opcode for ZP low clock, opcode: {:#04x}", static_cast<int>(info.opcode));
                    return false;
            }
        }
        else
        {
            spdlog::error("Unhandled cycle {} for ZP low clock, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
            return false;
        }
        return true;
        }




    void W65C02S::handlePhi2Low()
    {
        if (_stage < 2) // For any opcode, the first 2 cycles are reading opcode and first operand from PC.
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
            switch(it->second.addressingMode) 
            {
                case AddressingMode::IMM:
                case AddressingMode::REL:
                    _bus.setAddress(_pc);
                    break;
                case AddressingMode::ZP: // Zero Page
                    _bus.setAddress(_adl);
                    break;
                case AddressingMode::ZPX:                
                    _bus.setAddress(_adh);
                break;
                case AddressingMode::ABS: // Absolute
                {
                    if (it->second.opcode == Opcode::JSR)
                    {
                        if (_stage == 5)
                        {
                            _bus.setAddress(_pc);
                        }
                        else
                        {
                            _bus.setAddress(0x0100 + _sp); // Push to stack
                        }
                    }
                    else if (_stage == 2)
                    {
                        _bus.setAddress(_pc); // Read high byte of address
                    }
                    else
                    {
                        _bus.setAddress((_adh << 8) | _adl);
                    }
                }
                break;
                case AddressingMode::ABSX: // Absolute,X
                {
                    if (_stage == 2)
                    {
                        _bus.setAddress(_pc); // Read high byte of address
                    }
                    else
                    {
                        _bus.setAddress(((_adh << 8) | _adl) + _x);
                    }
                }
                break;
                case AddressingMode::ABSY: // Absolute,Y
                {
                    if (_stage == 2)
                    {
                        _bus.setAddress(_pc); // Read high byte of address
                    }
                    else
                    {
                        _bus.setAddress(((_adh << 8) | _adl) + _y);
                    }
                }
                break;
                case AddressingMode::IMP:
                {                    
                    _bus.setAddress(0x0100 + _sp); // Push to stack
                    // switch(it->second.opcode)
                    // {
                    //     case Opcode::PHA:
                    //     case Opcode::PLA:
                    //     case Opcode::PHP:
                    //     case Opcode::PLP:
                    //         _bus.setAddress(0x0100 + _sp); // Push to stack
                    //         break;
                    //     default:
                    //         spdlog::error("Unhandled implied addressing mode for opcode: {:#04x}", static_cast<int>(it->second.opcode));
                    //         break;
                    // }
                }
                break;
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
        bool inReset = _resetStage < 2;
        uint8_t rwb = (_stage == 0 || inReset) ? core::HIGH : getRWB();
        if (rwb == core::HIGH)
        {
            // Notify slaves that we are reading, set the data on the bus
            _bus.notifySlaves(rwb);
        }
        if (inReset)
        {
            if (_resetStage == 0)
            {
                _adl = fetchByte(); // Read low byte of reset vector
                spdlog::debug("CPU: Read reset vector low byte: {:#04x}", _adl);
                _resetStage++;
                _pc++;
                return;
            }
            else if (_resetStage == 1)
            {
                _adh = fetchByte(); // Read high byte of reset vector
                spdlog::debug("CPU: Read reset vector high byte: {:#04x}", _adh);
                _resetStage++;
                _pc = (_adh << 8) | _adl; // Set program counter to reset vector
                spdlog::debug("CPU: Reset complete, PC set to {:#04x}", _pc);
                return;
            }
        }
        if (_stage == 0) // First cycle is always fetch the opcode
        {
            uint8_t opcode = fetchByte();
            _ir = static_cast<Opcode>(opcode); // Read the instruction from the data bus
            _pc++; // Increment the program counter
            _stage++; // Move to the next stage
            return;
        }

        handleOpcode(rwb);
    }

    void W65C02S::handleOpcode(uint8_t rwb)
    {
        auto it = OpcodeMap.find(_ir);
        if (it == OpcodeMap.end()) {
            spdlog::error("CPU: unknown opcode: {:#04x}", static_cast<int>(_ir));
            return;
        }
        const OpcodeInfo& opcodeInfo = it->second;
        if (_stage == opcodeInfo.cycles - 1) // If last cycle, we finished fetching, execute the instruction
        {
            executeInstruction(opcodeInfo.opcode);
            _stage = 0; // Reset stage for next instruction
            if (shouldIncrementPC(opcodeInfo.addressingMode))
            {
                _pc++;
            }
        }
        else if (_stage == 1)
        {
            handleStage1(opcodeInfo);
            _stage++;
        }
        else if (_stage == 2)
        {
            handleStage2(opcodeInfo);
            _stage++;
        }
        else if (_stage == 3)
        {
            handleStage3(opcodeInfo);
            _stage++;
        }
        else if (_stage == 4)
        {
            handleStage4(opcodeInfo);
            _stage++;
        }
        else 
        {
            spdlog::debug("CPU: unhandled stage {} for opcode: {:#04x}", _stage, static_cast<int>(_ir));
            // Reset the stage if we are not in a valid state
            _stage = 0;
            return; // If we are not in a valid state, do nothing
        }
        if (rwb == core::LOW)
        {
            // Notify the slaves that we are writing, take values from bus
            _bus.notifySlaves(rwb);
        }
    }

    void W65C02S::handleStage1(const OpcodeInfo& info)
    {
        switch (info.opcode)
        {
            case Opcode::PHA:
            case Opcode::PLA:
            case Opcode::PHP:
            case Opcode::PLP:
            case Opcode::RTS:
                fetchByte(); // Fetch byte, ignore result
                break;
            default:                
                _adl = fetchByte(); // Fetch the low byte of the address
                spdlog::debug("CPU: Read address low byte: {:#04x}", _adl);
                _pc++;
                break;
        }
    }

    void W65C02S::handleStage2(const OpcodeInfo& info)
    {
        switch(info.opcode)
        {
            case Opcode::LDA_ZPX:
            case Opcode::LDY_ZPX:
            case Opcode::STY_ZPX:
            case Opcode::AND_ZPX:
                _adh = _adl + _x;
                break;
            case Opcode::PLA:
            case Opcode::PLP:
            case Opcode::RTS:
                _sp++;
                break;
            case Opcode::JSR:
                // Internal operation
                break;
            case Opcode::BEQ:
                _adh = fetchByte();
                if (_status & STATUS_ZERO) 
                {
                    int8_t offset = static_cast<int8_t>(_adl);
                    _pc += offset;
                } else 
                {
                    _stage = -1; // We increment outside this function. TODO: Update
                }
                break;
            case Opcode::BNE:
                _adh = fetchByte();
                if (!(_status & STATUS_ZERO))
                {
                    int8_t offset = static_cast<int8_t>(_adl);
                    _pc += offset;
                } else 
                {
                    _stage = -1; // We increment outside this function. TODO: Update
                }
                break;
            default:
                _adh = fetchByte(); // Fetch the high byte of the address                
                _pc++;
                break;
        }
    }
    
    void W65C02S::handleStage3(const OpcodeInfo& info)
    {
        switch(info.opcode)
        {
            case Opcode::JSR:
            {
                uint8_t pch = (_pc >> 8);
                _bus.setData(pch);
                _sp--;
            }
                break;
            case Opcode::RTS:
                _adl = fetchByte();
                _sp++;
                break;
            default:
                break;
        }
    }
    
    void W65C02S::handleStage4(const OpcodeInfo& info)
    {
        switch(info.opcode)
        {
            case Opcode::JSR:
            {
                uint8_t pcl = static_cast<uint8_t>(_pc);
                _bus.setData(pcl);
                _sp--;
            }
                break;
            case Opcode::RTS:
                _adh = fetchByte();
                _pc = (_adh << 8) | _adl;
                break;
            default:
                break;
        }
    }

    uint8_t W65C02S::fetchByte()
    {
        _bus.notifySlaves(core::READ);
        uint8_t data;
        _bus.getData(data); // Get data from the bus
        return data;
    }

    void W65C02S::writeByte(uint8_t data)
    {
        _bus.setData(data); // Set data on the bus
        _bus.notifySlaves(core::WRITE);
    }

    void W65C02S::executeInstruction(Opcode opcode)
    {   
        switch(opcode)
        {
            case Opcode::LDA_IMM:
            case Opcode::LDA_ZP:
            case Opcode::LDA_ZPX:
            case Opcode::LDA_ABS:
            case Opcode::LDA_ABSX:
            case Opcode::LDA_ABSY:
                _a = fetchByte();
                updateStatusFlags(_a);
                break;

            case Opcode::LDX_IMM:
            case Opcode::LDX_ZP:
            case Opcode::LDX_ZPY:
            case Opcode::LDX_ABS:
            case Opcode::LDX_ABSY:
                _x = fetchByte();
                updateStatusFlags(_x);
                break;

            case Opcode::STA_ABS:
                _bus.setData(_a);
                break;

            case Opcode::JMP_ABS:
            case Opcode::JSR:
                _adh = fetchByte();
                _pc = (_adh << 8) | _adl;
                break;

            case Opcode::BEQ:
            case Opcode::BNE:                
                break;


            case Opcode::PHA:
                _bus.setData(_a);
                _sp--;
                break;
            case Opcode::PLA:
                _a = fetchByte();
                break;
            case Opcode::PHP:
                _bus.setData(_status);
                _sp--;
                break;
            case Opcode::PLP:
                _status = fetchByte();
                break;
            case Opcode::RTS:
                _pc++;
                break;
            case Opcode::TAX:
                _x = _a;
                updateStatusFlags(_x);
                break;
            case Opcode::TXA:
                _a = _x;
                updateStatusFlags(_a);
                break;
            case Opcode::TAY:
                _y = _a;
                updateStatusFlags(_y);
                break;
            case Opcode::TYA:
                _a = _y;
                updateStatusFlags(_a);
                break;
            case Opcode::TXS:
                _sp = _x;
                break;
            case Opcode::TSX:
                _x = _sp;
                updateStatusFlags(_x);
                break;
            case Opcode::INX:
                _x++;
                updateStatusFlags(_x);
                break;
            case Opcode::DEX:
                _x--;
                updateStatusFlags(_x);
                break;                
            case Opcode::INY:
                _y++;
                updateStatusFlags(_y);
                break;
            case Opcode::DEY:
                _y--;
                updateStatusFlags(_y);
                break;

            case Opcode::AND_IMM:
            case Opcode::AND_ZP:
            case Opcode::AND_ZPX:
            case Opcode::AND_ABS:
            case Opcode::AND_ABSX:
            case Opcode::AND_ABSY:
            case Opcode::AND_INDX:
            case Opcode::AND_INDY:
            {
                uint8_t value = fetchByte();
                _a &= value; // Perform AND operation with the accumulator
                updateStatusFlags(_a);
            }
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
        switch(it->second.addressingMode)
        {
            case AddressingMode::IMM:
            case AddressingMode::REL:
            case AddressingMode::ZP:
            case AddressingMode::ZPX:
                return core::HIGH;
            case AddressingMode::IMP:
            {
                switch (it->second.opcode) 
                {
                    case Opcode::PHA:
                    case Opcode::PHP:
                        return core::LOW;
                    default:
                        return core::HIGH;
                }
            }
            case AddressingMode::ABS:
            case AddressingMode::ABSX:
            case AddressingMode::ABSY:
            {
                if (it->second.opcode == Opcode::JSR)
                {
                    if (_stage < 3 || _stage == 5)
                    {
                        return core::HIGH;
                    }
                    return core::LOW;
                }
                return _stage < 3 ? core::HIGH : it->second.rwb;
            }
            default:
                return core::LOW; // Write operation
        }
    }

    bool W65C02S::shouldIncrementPC(AddressingMode mode) const
    {
        switch(mode)
        {
            case AddressingMode::ABS:
            case AddressingMode::ABSX:
            case AddressingMode::ABSY:
            case AddressingMode::ZP:
            case AddressingMode::ZPX:
            case AddressingMode::ZPY:
            case AddressingMode::IMP:
            case AddressingMode::REL:
                return false; // Do not increment PC for absolute addressing modes
            default:
                return true; // Increment PC for other addressing modes
        }
    }
}