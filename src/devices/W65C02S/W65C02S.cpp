
#include "devices/W65C02S/W65C02S.h"
#include "core/defines.h"
#include "devices/W65C02S/opcodes.h"
#include "spdlog/spdlog.h"

#include <cinttypes>
#include <cstdint>
#include <iostream>

namespace EaterEmulator::devices
{
    W65C02S::W65C02S(std::shared_ptr<core::Bus> bus)
        : core::Device(bus)        
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

    void W65C02S::onClockStateChange(core::State state)
    {
        if (state == core::HIGH) 
        {
            handlePhi2High();
        } else {            
            handlePhi2Low();
        }
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
            spdlog::error("Unknown opcode: {:#04x}", static_cast<int>(_ir));
            return;
        }
        const auto opcodeInfo = it->second;
        const auto addressingMode = opcodeInfo.addressingMode;   
        if (_cycle == 0)
        {
            _bus->setAddress(_pc);            
            _started = true; // Make sure we start the CPU on the PHI2 low
        }
        else
        {
            bool handled = false;
            switch (addressingMode)
            {
                case AddressingMode::ACC:
                    handled = handleAccumulatorAddressing(opcodeInfo, core::LOW);
                    break;

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
                case AddressingMode::IND:
                    handled = handleIndirectAddressing(opcodeInfo, core::LOW);
                    break;
                case AddressingMode::INDX:
                case AddressingMode::INDY:
                    handled = handleIndirectIndexedAddressing(opcodeInfo, core::LOW);
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
                _interruptFromSW = false;
            }
            else
            {
                // Set reset vector to IRQ vector (Same vector for BRK)
                _interruptVector = IRQ_BRK_VECTOR;
                _interruptFromSW = true;
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
                spdlog::error("Unknown opcode: {:#04x}", static_cast<int>(_ir));
                return;
            }
            const auto opcodeInfo = it->second;
            const auto addressingMode = opcodeInfo.addressingMode;
            bool handled = false;
            switch (addressingMode)
            {
                case AddressingMode::ACC:
                    handled = handleAccumulatorAddressing(opcodeInfo, core::HIGH);
                    break;

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
                case AddressingMode::IND:
                    handled = handleIndirectAddressing(opcodeInfo, core::HIGH);
                    break;
                case AddressingMode::INDX:
                case AddressingMode::INDY:
                    handled = handleIndirectIndexedAddressing(opcodeInfo, core::HIGH);
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
    bool W65C02S::handleAccumulatorAddressing(const OpcodeInfo& info, core::State clockState)
    {
        if (clockState == core::LOW)
        {
            return handleAccumulatorLow(info);
        }
        return handleAccumulatorHigh(info);
    }
    bool W65C02S::handleAccumulatorLow(const OpcodeInfo& info)
    {
        return handleImpliedLow(info);
    }
    bool W65C02S::handleAccumulatorHigh(const OpcodeInfo& info)
    {
        if (_cycle == 1)
        {
            switch (info.opcode)
            {
                case Opcode::ASL_ACC:
                    doASL(true);
                    break;
                case Opcode::LSR_ACC:
                    doLSR(true);
                    break;
                case Opcode::ROL_ACC:
                    doROL(true);
                    break;
                case Opcode::ROR_ACC:
                    doROR(true);
                    break;
                case Opcode::INC_ACC:
                    doINC();
                    break;
                case Opcode::DEC_ACC:
                    doDEC();
                    break;
                default:
                    return false;
            }
            return true;
        }
        else
        {
            spdlog::error("Unhandled cycle in accumulator high addressing: {}", _cycle);
            return false;
        }
        return false;
    }

    bool W65C02S::handleImpliedAddressing(const OpcodeInfo& info, core::State clockState)
    {
        if (clockState == core::LOW)
        {
            return handleImpliedLow(info);
        }
        return handleImpliedHigh(info);
    }
    bool W65C02S::handleImpliedLow(const OpcodeInfo& info)
    {
        if (_cycle == 1)
        {
            return true;
        }
        else if (_cycle == 2)
        {
            _bus->setAddress(0x0100 + _sp);
            return true;
        }
        else if (_cycle == 3)
        {
            _bus->setAddress(0x0100 + _sp);
            return true;
        }
        else if (_cycle == 4)
        {
            _bus->setAddress(0x0100 + _sp);
            return true;
        }
        else if (_cycle == 5)
        {
            switch (info.opcode)
            {
                case Opcode::BRK:
                    _bus->setAddress(_interruptVector);
                    break;
                default:
                    _bus->setAddress(0x0100 + _sp);
                    break;
            }
            return true;
        }
        else if (_cycle == 6)
        {
            switch (info.opcode)
            {
                case Opcode::BRK:
                    _bus->setAddress(_interruptVector + 1);
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
                case Opcode::RTI:
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
                case Opcode::RTI:
                    _status = fetchByte();
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
                    {
                        auto status = _interruptFromSW ? _status | devices::STATUS_BREAK : _status;
                        writeByte(status); // Push status register
                        _sp--;
                    }
                    break;
                case Opcode::RTS:
                    _pc = (fetchByte() << 8) | _adl;
                    break;
                case Opcode::RTI:
                    _adl = fetchByte();
                    _sp++;
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
                case Opcode::RTI:
                    _pc = (fetchByte() << 8) | _adl;
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
                    if (_interruptVector == IRQ_BRK_VECTOR)
                    {
                        _status |= devices::STATUS_INTERRUPT;
                    }
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
            _bus->setAddress(_pc++);
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
                    _y = fetchByte();
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
                    break;
                case Opcode::ADC_IMM:
                    doADC();
                    break;
                case Opcode::SBC_IMM:
                    doSBC();
                    break;
                case Opcode::CMP_IMM:
                case Opcode::CPX_IMM:
                case Opcode::CPY_IMM:
                    doCMP();
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
            _bus->setAddress(_pc++);
        }
        else if (_cycle == 2)
        {
            switch (info.opcode)
            {
                case Opcode::JSR:
                    _bus->setAddress(0x0100 + _sp);
                    break;
                default:
                    // fetch high byte of address, increment PC
                    _bus->setAddress(_pc++);
                    break;
            }
        }
        else if (_cycle == 3)
        {
            switch (info.opcode)
            {
                case Opcode::JSR:
                    _bus->setAddress(0x0100 + _sp);
                    break;
                default:
                    // read from effective address
                    _bus->setAddress((_adh << 8) | _adl);
                    break;
            }
            
        }
        else if (_cycle == 4)
        {
            _bus->setAddress(0x0100 + _sp);
        }
        else if (_cycle == 5)
        {
            _bus->setAddress(_pc);
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
                case Opcode::ADC_ABS:
                case Opcode::ADC_ABSX:
                case Opcode::ADC_ABSY:
                    doADC();
                    break;
                case Opcode::SBC_ABS:
                case Opcode::SBC_ABSX:
                case Opcode::SBC_ABSY:
                    doSBC();
                    break;
                case Opcode::CMP_ABS:
                case Opcode::CMP_ABSX:
                case Opcode::CMP_ABSY:
                case Opcode::CPX_ABS:
                case Opcode::CPY_ABS:
                    doCMP();
                    break;

                case Opcode::BIT_ABS:
                    doBIT();
                    break;

                case Opcode::ASL_ABS:
                case Opcode::ASL_ABSX:
                    doASL(false);
                    break;
                case Opcode::LSR_ABS:
                case Opcode::LSR_ABSX:
                    doLSR(false);
                    break;
                case Opcode::ROL_ABS:
                case Opcode::ROL_ABSX:
                    doROL(false);
                    break;
                case Opcode::ROR_ABS:
                case Opcode::ROR_ABSX:
                    doROR(false);
                    break;

                // Read-modify-write instructions
                // Write instructions
                case Opcode::STA_ABS:
                case Opcode::STA_ABSX:
                case Opcode::STA_ABSY:
                    writeByte(_a);
                    break;
                case Opcode::STX_ABS:
                    writeByte(_x);
                    break;
                case Opcode::STY_ABS:
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
            _bus->setAddress(_pc++);
        }
        else if (_cycle == 2)
        {
            // fetch high byte of address, increment PC
            _bus->setAddress(_pc++);
        }
        else if (_cycle == 3)
        {
            // read from effective address
            _bus->setAddress(((_adh << 8) | _adl) + indexingRegister);
        }
        else if (_cycle == 4)
        {
            // re-read from effective address
            _bus->setAddress(((_adh << 8) | _adl) + indexingRegister);
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
        _bus->setAddress(_pc);
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
            bool isNegative = _status & devices::STATUS_NEGATIVE;
            bool isCarry = _status & devices::STATUS_CARRY;
            bool isOverflow = _status & devices::STATUS_OVERFLOW;
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
                case Opcode::BPL:
                    if (!isNegative)
                    {
                        _pc += static_cast<int8_t>(_adl);
                    }
                    break;
                case Opcode::BMI:
                    if (isNegative)
                    {
                        _pc += static_cast<int8_t>(_adl);
                    }
                    break;
                case Opcode::BCS:
                    if (isCarry)
                    {
                        _pc += static_cast<int8_t>(_adl);
                    }
                    break;
                case Opcode::BCC:
                    if (!isCarry)
                    {
                        _pc += static_cast<int8_t>(_adl);
                    }
                    break;
                case Opcode::BVS:
                    if (!isOverflow)
                    {
                        _pc += static_cast<int8_t>(_adl);
                    }
                    break;
                case Opcode::BVC:
                    if (!isOverflow)
                    {
                        _pc += static_cast<int8_t>(_adl);
                    }
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
                case Opcode::BPL:
                case Opcode::BMI:
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
            _bus->setAddress(_pc++);
        }
        else if (_cycle == 2)
        {
            // read from effective address
            _bus->setAddress( _adl);
        }
        else if (_cycle == 3)
        {
            _bus->setAddress( _adl);
        }
        else if (_cycle == 4)
        {
            _bus->setAddress( _adl);
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
                    _a = fetchByte();
                    updateStatusFlags(_a);
                    break;
                case Opcode::LDX_ZP:
                    _x = fetchByte();
                    updateStatusFlags(_x);
                    break;
                case Opcode::LDY_ZP:
                    _y = fetchByte();
                    updateStatusFlags(_y);
                    break;
                case Opcode::AND_ZP:
                    doAND();
                    break;
                case Opcode::ORA_ZP:
                    doORA();
                    break;
                case Opcode::EOR_ZP:
                    doEOR();
                    break;
                case Opcode::ADC_ZP:
                    doADC();
                    break;
                case Opcode::SBC_ZP:
                    doSBC();
                    break;
                case Opcode::CMP_ZP:
                case Opcode::CPX_ZP:
                case Opcode::CPY_ZP:
                    doCMP();
                    break;                

                case Opcode::BIT_ZP:
                    doBIT();
                    break;
                case Opcode::ASL_ZP:
                    doASL(false);
                    break;
                case Opcode::LSR_ZP:
                    doLSR(false);
                    break;
                case Opcode::ROL_ZP:
                    doROL(false);
                    break;
                case Opcode::ROR_ZP:
                    doROR(false);
                    break;

                // Read-modify-write instructions
                // Write instructions
                case Opcode::STA_ZP:
                    writeByte(_a);
                    break;
                case Opcode::STX_ZP:
                    writeByte(_x);
                    break;
                case Opcode::STY_ZP:
                    writeByte(_y);
                    break;
                default:                    
                    spdlog::error("Unhandled opcode for ZP high clock, opcode: {:#04x}", static_cast<int>(info.opcode));
                    return false;
            }
        }
        else if (_cycle == 3)
        {
            // Do Nothing
        }
        else if (_cycle == 4)
        {
            // Do Nothing
        }
        else
        {
            spdlog::error("Unhandled cycle {} for ZP high clock, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
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
                _bus->setAddress(_pc++);
            }
            else if (_cycle == 2)
            {
                // read from effective address
                _bus->setAddress( _adl);
            }
            else if (_cycle == 3)
            {
                _bus->setAddress(_adh);
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
                case Opcode::ADC_ZPX:
                    doADC();
                    break;
                case Opcode::SBC_ZPX:
                    doSBC();
                    break;
                case Opcode::CMP_ZPX:
                    doCMP();
                    break;
                    
                case Opcode::ASL_ZPX:
                    doASL(false);
                    break;
                case Opcode::LSR_ZPX:
                    doLSR(false);
                    break;
                case Opcode::ROL_ZPX:
                    doROL(false);
                    break;
                case Opcode::ROR_ZPX:
                    doROR(false);
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
                    spdlog::error("Unhandled opcode for ZP indexed high clock, opcode: {:#04x}", static_cast<int>(info.opcode));
                    return false;
            }
        }
        else if (_cycle == 4)
        {
            // Do nothing
        }
        else
        {
            spdlog::error("Unhandled cycle {} for ZP indexed high, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
            return false;
        }
        return true;
    }
    bool W65C02S::handleIndirectAddressing(const OpcodeInfo& info, core::State clockState)
    {
        if (clockState == core::LOW)
        {
            return handleIndirectLow(info);
        }
        return handleIndirectHigh(info);
    }
    bool W65C02S::handleIndirectLow(const OpcodeInfo& info)
    {
        if (_cycle == 1) {
            _bus->setAddress(_pc++);
        } else if (_cycle == 2) {
            _bus->setAddress(_pc++);
        } else if (_cycle == 3) {
            _bus->setAddress(((_adh << 8) | _adl));
        } else if (_cycle == 4) {
            _bus->setAddress(((_adh << 8) | (_adl + 1)));
        } else {
            spdlog::error("Unhandled cycle {} for indirect indexed low clock, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
            return false;
        }
        return true;
    }
    bool W65C02S::handleIndirectHigh(const OpcodeInfo& info)
    {
        if (_cycle == 1) {
            _adl = fetchByte();
        } else if (_cycle == 2) {
            _adh = fetchByte();
        } else if (_cycle == 3) {
            _add = fetchByte();
        } else if (_cycle == 5) {
            switch (info.opcode) {
                case Opcode::JMP_IND:
                    _pc = (fetchByte() << 8) | _add;                
                    break;
                default:
                    spdlog::error("Unhandled opcode for IND high clock, opcode: {:#04x}", static_cast<int>(info.opcode));
                    return false;
            }
        } else {
            spdlog::error("Unhandled cycle {} for IND high clock, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
            return false;
        }
        return true;                    
    }

    bool W65C02S::handleIndirectIndexedAddressing(const OpcodeInfo& info, core::State clockState)
    {
        if (clockState == core::LOW)
        {
            return handleIndirectIndexedLow(info);
        }
        return handleIndirectIndexedHigh(info);
    }

    bool W65C02S::handleIndirectIndexedLow(const OpcodeInfo& info)
    {
        if (_cycle == 1) {
            _bus->setAddress(_pc++);
        } else if (_cycle == 2) {
            _bus->setAddress(_add);
        } else if (_cycle == 3) {
            _bus->setAddress(_add & 0xFF);
        } else if (_cycle == 4) {
            _bus->setAddress((_add + 1) & 0xFF);
        } else if (_cycle == 5) {
            _bus->setAddress(((_adh << 8) | _adl));
        } else {
            spdlog::error("Unhandled cycle {} for indirect indexed low clock, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
            return false;
        }
        return true;
    }

    bool W65C02S::handleIndirectIndexedHigh(const OpcodeInfo& info)
    {
        uint8_t& indexingRegister = info.addressingMode == AddressingMode::INDX ? _x : _y;
        if (_cycle == 1) {
            _add = fetchByte();
        } else if (_cycle == 2) {
            _add = fetchByte() + indexingRegister;
        } else if (_cycle == 3) {
            _adl = fetchByte();
        } else if (_cycle == 4) {
            _adh = fetchByte();
        } else if (_cycle == 5) {
            switch (info.opcode) {
                case Opcode::LDA_INDX:
                case Opcode::LDA_INDY:
                    _a = fetchByte();
                    updateStatusFlags(_a);
                    break;
                case Opcode::AND_INDX:
                case Opcode::AND_INDY:
                    doAND();
                    break;
                case Opcode::ORA_INDX:
                case Opcode::ORA_INDY:
                    doORA();
                    break;
                case Opcode::EOR_INDX:
                case Opcode::EOR_INDY:
                    doEOR();
                    break;
                case Opcode::ADC_INDX:
                case Opcode::ADC_INDY:
                    doADC();
                    break;
                case Opcode::SBC_INDX:
                case Opcode::SBC_INDY:
                    doSBC();
                    break;
                case Opcode::CMP_INDX:
                case Opcode::CMP_INDY:
                    doCMP();
                    break;
                case Opcode::STA_INDX:
                case Opcode::STA_INDY:
                    writeByte(_a);
                    break;
                default:
                    spdlog::error("Unhandled opcode for indirect indexed high clock, opcode: {:#04x}", static_cast<int>(info.opcode));
                    return false;
            }
        } else {
            spdlog::error("Unhandled cycle {} for indirect indexed high clock, opcode: {:#04x}", _cycle, static_cast<int>(info.opcode));
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
        uint8_t operand = fetchByte();
        uint16_t result = static_cast<uint16_t>(_a) + static_cast<uint16_t>(operand) + static_cast<uint16_t>(_status & STATUS_CARRY);
        _a = static_cast<uint8_t>(result);
        if (result > 0xFF) {
            _status |= STATUS_CARRY; // Set carry flag if overflow
        } else {
            _status &= ~STATUS_CARRY;
        }
        updateStatusFlags(_a);
    }
    void W65C02S::doSBC()
    {
        uint8_t operand = fetchByte();
        uint16_t result = static_cast<uint16_t>(_a) - static_cast<uint16_t>(operand) - (1 - static_cast<uint16_t>(_status & STATUS_CARRY));
        _a = static_cast<uint8_t>(result);
        if (result > 0xFF) {
            _status &= ~STATUS_CARRY; // Clear carry flag if overflow
        } else {
            _status |= STATUS_CARRY;
        }
        updateStatusFlags(_a);
    }

    void W65C02S::doCMP()
    {
        auto* reg = &_a;
        switch(_ir)
        {
            case Opcode::CPY_ABS:
            case Opcode::CPY_IMM:            
            case Opcode::CPY_ZP:
                reg = &_y;
                break;
            case Opcode::CPX_ABS:
            case Opcode::CPX_IMM:            
            case Opcode::CPX_ZP:
                reg = &_x;
                break;
            default:
                break;
        }
        uint8_t operand = fetchByte();
        uint16_t result = static_cast<uint16_t>(*reg) - static_cast<uint16_t>(operand);
        if (result > 0xFF) {
            _status &= ~STATUS_CARRY; // Clear carry flag if overflow
        } else {
            _status |= STATUS_CARRY;
        }
        updateStatusFlags(static_cast<uint8_t>(result));
    }

    void W65C02S::doBIT()
    {
        uint8_t value = fetchByte();
        _status &= ~(STATUS_OVERFLOW | STATUS_ZERO | STATUS_NEGATIVE);
        _status |= (value & 0x40) ? STATUS_OVERFLOW : 0;
        _status |= (value & 0x80) ? STATUS_NEGATIVE : 0;
        if ((value & _a) == 0) {
            _status |= STATUS_ZERO;
        }
        spdlog::debug("CPU: BIT operation, status updated: {:#04x}", static_cast<int>(_status));
    }

    void W65C02S::doASL(bool accumulator)
    {
        uint8_t value = _a;
        if (!accumulator)
        {
            value = fetchByte();
        }
        _status &= ~(STATUS_ZERO | STATUS_NEGATIVE);
        _status |= (value & 0x80) ? STATUS_CARRY : 0;
        value <<= 1;
        _a = value;
        updateStatusFlags(_a);
    }

    void W65C02S::doLSR(bool accumulator)
    {
        uint8_t value = _a;
        if (!accumulator)
        {
            value = fetchByte();
        }
        _status &= ~(STATUS_ZERO | STATUS_NEGATIVE);
        _status |= (value & 0x01) ? STATUS_CARRY : 0;
        value >>= 1;
        _a = value;
        updateStatusFlags(_a);
    }

    void W65C02S::doROL(bool accumulator)
    {
        uint8_t value = _a;
        if (!accumulator)
        {
            value = fetchByte();
        }
        bool carry = value & 0x80; // Check if the highest bit is set
        value = (value << 1) | (_status & STATUS_CARRY); // Shift left and add carry
        _status &= ~STATUS_CARRY; // Clear carry flag
        if (carry) {
            _status |= STATUS_CARRY; // Set carry flag if it was set before
        }
        _a = value;
        updateStatusFlags(_a);
    }

    void W65C02S::doROR(bool accumulator)
    {
        uint8_t value = _a;
        if (!accumulator)
        {
            value = fetchByte();
        }
        bool carry = value & 0x01; // Check if the lowest bit is set
        value = (_status & STATUS_CARRY) << 7 | (value >> 1); // Shift right and add carry
        _status &= ~STATUS_CARRY; // Clear carry flag
        if (carry) {
            _status |= STATUS_CARRY; // Set carry flag if it was set before
        }
        _a = value;
        updateStatusFlags(_a);
    }

    void W65C02S::doINC()
    {
        _a++;
        updateStatusFlags(_a);
    }

    void W65C02S::doDEC()
    {
        _a--;
        updateStatusFlags(_a);
    }

    uint8_t W65C02S::fetchByte()
    {
        _bus->notifySlaves(core::READ);
        uint8_t data;
        _bus->getData(data); // Get data from the bus
        return data;
    }

    void W65C02S::writeByte(uint8_t data)
    {
        _bus->setData(data); // Set data on the bus
        _bus->notifySlaves(core::WRITE);
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