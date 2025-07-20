
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
        _pc = RESET_VECTOR; // Program Counter - Reset vector
        _status = 0; // Processor Status
        _adl = 0; // Address Low Byte
        _adh = 0; // Address High Byte
        _resetStage = 0;
    }

    void W65C02S::handleClockStateChange(core::State state)
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

    void W65C02S::setIRQ(core::State state)
    {
        _irq = state;
    }

    void W65C02S::setNMI(core::State state)
    {
        _nmi = state;
    }


    void W65C02S::handlePhi2Low()
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
                case AddressingMode::IMP:
                    handled = handleImpliedAddressing(opcodeInfo, core::LOW);
                    break;
                    
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

                case AddressingMode::REL:
                    handled = handleRelativeAddressing(opcodeInfo, core::LOW);
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

    void W65C02S::handlePhi2High()
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

            // Check if interrupt (NMI) was requested
            if (_nmi == core::LOW)
            {
                // Inject BRK to IR
                _ir = Opcode::BRK;
                // Set reset vector to NMI vector
                _interruptVector = NMI_VECTOR;
            }
            // Check if interrupt (IRQ) was requested and interrupt bit is cleared
            else if (_irq == core::LOW && (_status & devices::STATUS_INTERRUPT) == 0)            
            {
                // Inject BRK to IR
                // Set reset vector to IRQ vector
                _interruptVector = IRQ_BRK_VECTOR;
                _ir = Opcode::BRK;
            }
            else
            {
                // Set reset vector to IRQ vector (Same vector for BRK)
                _interruptVector = IRQ_BRK_VECTOR;
                uint8_t opcode = fetchByte();
                _ir = static_cast<Opcode>(opcode); // Read the instruction from the data bus
                _pc++;
            }
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
                case AddressingMode::IMP:
                    handled = handleImpliedAddressing(opcodeInfo, core::HIGH);
                    break;

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

                case AddressingMode::REL:
                    handled = handleRelativeAddressing(opcodeInfo, core::HIGH);
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

    bool W65C02S::handleImpliedAddressing(const OpcodeInfo& info, core::State clockState)
    {
        if (clockState == core::LOW)
        {
            return handleImpliedLow(info);
        }
        return handleImpliedHigh(info);
    }
    bool W65C02S::handleImpliedLow([[maybe_unused]]const OpcodeInfo& info)
    {
        if (_cycle == 1)
        {
            return true;
        }
        else if (_cycle == 2)
        {
            _bus.setAddress(0x0100 + _sp);
            return true;
        }
        else if (_cycle == 3)
        {
            _bus.setAddress(0x0100 + _sp);
            return true;
        }
        else if (_cycle == 4)
        {
            _bus.setAddress(0x0100 + _sp);
            return true;
        }
        else if (_cycle == 5)
        {
            switch (info.opcode)
            {
                case Opcode::BRK:
                    _bus.setAddress(_interruptVector);
                    break;
                default:
                    _bus.setAddress(0x0100 + _sp);
                    break;
            }
            return true;
        }
        else if (_cycle == 6)
        {
            switch (info.opcode)
            {
                case Opcode::BRK:
                    _bus.setAddress(_interruptVector + 1);
                    break;
                default:
                    break;
            }
            return true;
        }
        else
        {
            spdlog::error("Unhandled cycle in implied low addressing: {}", _cycle);
            return false;
        }
        return false;
    }
    bool W65C02S::handleImpliedHigh(const OpcodeInfo& info)
    {
        if (_cycle == 1)
        {
            switch (info.opcode)
            {
                case Opcode::BRK: // Internal operation                    
                case Opcode::NOP:
                    break;
                case Opcode::CLC:
                    _status &= ~devices::STATUS_CARRY; // Clear carry flag
                    break;
                case Opcode::SEC:
                    _status |= devices::STATUS_CARRY; // Set carry flag
                    break;
                case Opcode::CLI:
                    _status &= ~devices::STATUS_INTERRUPT; // Clear interrupt disable flag
                    break;
                case Opcode::SEI:
                    _status |= devices::STATUS_INTERRUPT; // Set interrupt disable flag
                    break;
                case Opcode::CLV:
                    _status &= ~devices::STATUS_OVERFLOW; // Clear overflow flag
                    break;
                case Opcode::CLD:
                    _status &= ~devices::STATUS_DECIMAL; // Clear decimal mode flag
                    break;
                case Opcode::SED:
                    _status |= devices::STATUS_DECIMAL; // Set decimal mode flag
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
                
                default:
                    break;
            
            }
            return true;
        }
        else if (_cycle == 2)
        {
            switch (info.opcode)
            {
                case Opcode::BRK:
                    writeByte(static_cast<uint8_t>(_pc >> 8)); // Push return address high byte
                    _sp--;
                    break;
                case Opcode::PHA:
                    writeByte(_a); // Push accumulator to stack
                    _sp--;
                    break;
                case Opcode::PLA:
                case Opcode::PLP:
                case Opcode::RTS:
                case Opcode::RTI:
                    _sp++;
                    break;
                case Opcode::PHP:
                    writeByte(_status);
                    _sp--;
                    break;
                default:
                    break;
            
            }
            return true;
        }
        else if (_cycle == 3)
        {
            switch (info.opcode)
            {
                case Opcode::BRK:
                    writeByte(static_cast<uint8_t>(_pc)); // Push return address low byte
                    _sp--;
                    break;
                case Opcode::PLA:
                    _a = fetchByte(); // Pull accumulator from stack
                    updateStatusFlags(_a);
                    break;
                case Opcode::PLP:
                    _status = fetchByte(); // Pull status from stack
                    _status &= ~devices::STATUS_BREAK; // Clear break flag
                    break;
                case Opcode::RTS:
                    _adl = fetchByte();
                    _sp++;
                    break;
                default:
                    break;
            }
            return true;
        }
        else if (_cycle == 4)
        {
            switch (info.opcode)
            {
                case Opcode::BRK:
                    writeByte(_status); // Push status register
                    _sp--;
                    break;
                case Opcode::RTS:
                    _pc = (fetchByte() << 8) | _adl;
                    break;
                default:
                    break; 
            }
            return true;
        }
        else if (_cycle == 5)
        {
            switch (info.opcode)
            {
                case Opcode::BRK:
                    _adl = fetchByte();
                    break;
                case Opcode::RTS:
                    _pc++;
                    break;
                default:
                    break; 
            }
            return true;
        }
        else if (_cycle == 6)
        {
            switch (info.opcode)
            {
                case Opcode::BRK:
                    _pc = (fetchByte() << 8) | _adl;
                    break;
                default:
                    break;
            }
            return true;
        }
        else
        {
            spdlog::error("Unhandled cycle in implied low addressing: {}", _cycle);
            return false;
        }
        return false;  
    }    

    bool W65C02S::handleImmediateAddressing(const OpcodeInfo& info, core::State clockState)
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
                case Opcode::AND_IMM:
                    doAND();
                    break;
                case Opcode::ORA_IMM:
                    doORA();
                    break;
                case Opcode::EOR_IMM:
                    doEOR();
                    updateStatusFlags(_a);
                    break;
                default:
                    return false;
            }
            return true;
        }
        return false;
    }

    bool W65C02S::handleAbsoluteAddressing(const OpcodeInfo& info, core::State clockState)
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
            switch (info.opcode)
            {
                case Opcode::JSR:
                    _bus.setAddress(0x0100 + _sp);
                    break;
                default:
                    // fetch high byte of address, increment PC
                    _bus.setAddress(_pc++);
                    break;
            }
        }
        else if (_cycle == 3)
        {
            switch (info.opcode)
            {
                case Opcode::JSR:
                    _bus.setAddress(0x0100 + _sp);
                    break;
                default:
                    // read from effective address
                    _bus.setAddress((_adh << 8) | _adl);
                    break;
            }
            
        }
        else if (_cycle == 4)
        {
            _bus.setAddress(0x0100 + _sp);
        }
        else if (_cycle == 5)
        {
            _bus.setAddress(_pc);
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
            
            switch (info.opcode)
            {
                case Opcode::JSR:                
                    break;
                case Opcode::JMP_ABS:
                    // copy low address byte to PCL, fetch high address byte to PCH
                    _pc = (fetchByte() << 8) | _adl;
                    break;
                default:
                    // fetch high byte of address, increment PC
                    _adh = fetchByte();
                    break;
            }
        }
        else if (_cycle == 3)
        {
            switch (info.opcode)
            {
                case Opcode::JSR:
                    writeByte(_pc >> 8); // Push PCH on stack
                    _sp--;
                    break;

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
                case Opcode::AND_ABS:
                case Opcode::AND_ABSX:
                case Opcode::AND_ABSY:
                    doAND();
                    break;
                case Opcode::ORA_ABS:
                case Opcode::ORA_ABSX:
                case Opcode::ORA_ABSY:
                    doORA();
                    break;
                case Opcode::EOR_ABS:
                case Opcode::EOR_ABSX:
                case Opcode::EOR_ABSY:
                    doEOR();
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
                    break;
            }
        }
        else if (_cycle == 4)
        {
            switch (info.opcode)
            {
                case Opcode::JSR:
                    writeByte(static_cast<uint8_t>(_pc)); // Push PCL on stack
                    _sp--;
                    break;
                default:
                    break;
            }
        }
        else if (_cycle == 5)
        {
            switch (info.opcode)
            {
                case Opcode::JSR:
                    _pc = fetchByte() << 8 | _adl;
                    break;
                default:
                    break;
            }
        }
        return true;
    }

    bool W65C02S::handleAbsoluteIndexedAddressing(const OpcodeInfo& info, core::State clockState)
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

    bool W65C02S::handleRelativeAddressing(const OpcodeInfo& info, core::State clockState)
    {
        if (clockState == core::LOW)
        {
            return handleRelativeLow(info);
        }
        return handleRelativeHigh(info);
    }
    bool W65C02S::handleRelativeLow([[maybe_unused]]const OpcodeInfo& info)
    {
        _bus.setAddress(_pc);
        return true;
    }
    bool W65C02S::handleRelativeHigh(const OpcodeInfo& info)
    {
        if (_cycle == 1)
        {
            _adl = fetchByte();
            _pc++;
        }
        else if (_cycle == 2)
        {
            bool isZero = _status & devices::STATUS_ZERO;
            switch (info.opcode)
            {
                case Opcode::BEQ:
                    if (isZero)
                    {
                        _pc += static_cast<int8_t>(_adl);
                    }
                    break;
                case Opcode::BNE:
                    if (!isZero)
                    {
                        _pc += static_cast<int8_t>(_adl);
                    }
                    break;
                case Opcode::BCS:
                case Opcode::BCC:
                case Opcode::BVS:
                case Opcode::BVC:
                    break;
                default:
                    break;
            }
        }
        else if (_cycle == 3)
        {
            switch (info.opcode)
            {
                case Opcode::BEQ:
                case Opcode::BNE:
                case Opcode::BCS:
                case Opcode::BCC:
                case Opcode::BVS:
                case Opcode::BVC:
                    // _pc++;
                    break;
                default:
                    return true;
            }
        }
        else
        {
            spdlog::error("Unhandled cycle {} for relative addressing, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
            return false;
        }
        return true;
    }


    bool W65C02S::handleZeroPageAddressing(const OpcodeInfo& info, core::State clockState)
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
                case Opcode::AND_ZP:
                case Opcode::AND_ZPX:
                    doAND();
                    break;
                case Opcode::ORA_ZP:
                case Opcode::ORA_ZPX:
                    doORA();
                    break;
                case Opcode::EOR_ZP:
                case Opcode::EOR_ZPX:
                    doEOR();
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

        bool W65C02S::handleZeroPageIndexedAddressing(const OpcodeInfo& info, core::State clockState)
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

                case Opcode::AND_ZPX:
                    doAND();
                    break;
                case Opcode::ORA_ZPX:
                    doORA();
                    break;
                case Opcode::EOR_ZPX:
                    doEOR();
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


    void W65C02S::doAND()
    {
        _a &= fetchByte();
        updateStatusFlags(_a);
    }
    void W65C02S::doORA()
    {
        _a |= fetchByte();
        updateStatusFlags(_a);
    }
    void W65C02S::doEOR()
    {
        _a ^= fetchByte();
        updateStatusFlags(_a);
    }
    void W65C02S::doADC()
    {

    }
    void W65C02S::doSBC()
    {

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
}