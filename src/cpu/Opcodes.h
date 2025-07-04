#pragma once

#include <cstddef>
#include <unordered_map>
#include <cstdint>

namespace EaterEmulator 
{

    // Basic opcodes for the 6502 CPU
    enum class Opcode : uint8_t 
    {
        // Load/Store Operations
        LDA_IMM = 0xA9, // LDA #$nn Immediate
        LDA_ZP = 0xA5,  // LDA $nn Zero Page
        LDA_ZPX = 0xB5, // LDA $nn,X Zero Page,X
        LDA_ABS = 0xAD, // LDA $nnnn Absolute
        LDA_ABSX = 0xBD, // LDA $nnnn,X Absolute,X
        LDA_ABSY = 0xB9, // LDA $nnnn,Y Absolute,Y
        LDA_INDX = 0xA1, // LDA ($nn,X) Indirect,X
        LDA_INDY = 0xB1, // LDA ($nn),Y Indirect,Y

        LDX_IMM = 0xA2, // LDX #$nn Immediate
        LDX_ZP = 0xA6,  // LDX $nn Zero Page
        LDX_ZPY = 0xB6, // LDX $nn,Y Zero Page,Y
        LDX_ABS = 0xAE, // LDX $nnnn Absolute
        LDX_ABSY = 0xBE, // LDX $nnnn,Y Absolute,Y

        LDY_IMM = 0xA0, // LDY #$nn Immediate
        LDY_ZP = 0xA4,  // LDY $nn Zero Page
        LDY_ZPX = 0xB4, // LDY $nn,X Zero Page,X
        LDY_ABS = 0xAC, // LDY $nnnn Absolute
        LDY_ABSX = 0xBC, // LDY $nnnn,X Absolute,X

        STA_ZP = 0x85,  // STA $nn Zero Page
        STA_ZPX = 0x95, // STA $nn,X Zero Page,X
        STA_ABS = 0x8D, // STA $nnnn Absolute
        STA_ABSX = 0x9D, // STA $nnnn,X Absolute,X
        STA_ABSY = 0x99, // STA $nnnn,Y Absolute,Y
        STA_INDX = 0x81, // STA ($nn,X) Indirect,X
        STA_INDY = 0x91, // STA ($nn),Y Indirect,Y

        STX_ZP = 0x86,  // STX $nn Zero Page
        STX_ZPY = 0x96, // STX $nn,Y Zero Page,Y
        STX_ABS = 0x8E, // STX $nnnn Absolute

        STY_ZP = 0x84,  // STY $nn Zero Page
        STY_ZPX = 0x94, // STY $nn,X Zero Page,X
        STY_ABS = 0x8C, // STY $nnnn Absolute

        // Register Transfers
        TAX = 0xAA, // Transfer Accumulator to X
        TXA = 0x8A, // Transfer X to Accumulator
        DEX = 0xCA, // Decrement X
        INX = 0xE8, // Increment X
        TAY = 0xA8, // Transfer Accumulator to Y
        TYA = 0x98, // Transfer Y to Accumulator
        DEY = 0x88, // Decrement Y
        INY = 0xC8, // Increment Y

        // Stack Operations
        TSX = 0xBA, // Transfer Stack Pointer to X
        TXS = 0x9A, // Transfer X to Stack Pointer
        PHA = 0x48, // Push Accumulator
        PLA = 0x68, // Pull Accumulator
        PHP = 0x08, // Push Processor Status
        PLP = 0x28, // Pull Processor Status

        // Logical
        AND_IMM = 0x29, // AND #$nn Immediate
        AND_ZP = 0x25,  // AND $nn Zero Page
        AND_ZPX = 0x35, // AND $nn,X Zero Page,X
        AND_ABS = 0x2D, // AND $nnnn Absolute
        AND_ABSX = 0x3D, // AND $nnnn,X Absolute,X
        AND_ABSY = 0x39, // AND $nnnn,Y Absolute,Y
        AND_INDX = 0x21, // AND ($nn,X) Indirect,X
        AND_INDY = 0x31, // AND ($nn),Y Indirect,Y

        EOR_IMM = 0x49, // EOR #$nn Immediate
        EOR_ZP = 0x45,  // EOR $nn Zero Page
        EOR_ZPX = 0x55, // EOR $nn,X Zero Page,X
        EOR_ABS = 0x4D, // EOR $nnnn Absolute
        EOR_ABSX = 0x5D, // EOR $nnnn,X Absolute,X
        EOR_ABSY = 0x59, // EOR $nnnn,Y Absolute,Y
        EOR_INDX = 0x41, // EOR ($nn,X) Indirect,X
        EOR_INDY = 0x51, // EOR ($nn),Y Indirect,Y

        ORA_IMM = 0x09, // ORA #$nn Immediate
        ORA_ZP = 0x05,  // ORA $nn Zero Page
        ORA_ZPX = 0x15, // ORA $nn,X Zero Page,X
        ORA_ABS = 0x0D, // ORA $nnnn Absolute
        ORA_ABSX = 0x1D, // ORA $nnnn,X Absolute,X
        ORA_ABSY = 0x19, // ORA $nnnn,Y Absolute,Y
        ORA_INDX = 0x01, // ORA ($nn,X) Indirect,X
        ORA_INDY = 0x11, // ORA ($nn),Y Indirect,Y

        BIT_ZP = 0x24,  // BIT $nn Zero Page
        BIT_ABS = 0x2C, // BIT $nnnn Absolute

        // Arithmetic
        ADC_IMM = 0x69, // ADC #$nn Immediate
        ADC_ZP = 0x65,  // ADC $nn Zero Page
        ADC_ZPX = 0x75, // ADC $nn,X Zero Page,X
        ADC_ABS = 0x6D, // ADC $nnnn Absolute
        ADC_ABSX = 0x7D, // ADC $nnnn,X Absolute,X
        ADC_ABSY = 0x79, // ADC $nnnn,Y Absolute,Y
        ADC_INDX = 0x61, // ADC ($nn,X) Indirect,X
        ADC_INDY = 0x71, // ADC ($nn),Y Indirect,Y

        SBC_IMM = 0xE9, // SBC #$nn Immediate
        SBC_ZP = 0xE5,  // SBC $nn Zero Page
        SBC_ZPX = 0xF5, // SBC $nn,X Zero Page,X
        SBC_ABS = 0xED, // SBC $nnnn Absolute
        SBC_ABSX = 0xFD, // SBC $nnnn,X Absolute,X
        SBC_ABSY = 0xF9, // SBC $nnnn,Y Absolute,Y
        SBC_INDX = 0xE1, // SBC ($nn,X) Indirect,X
        SBC_INDY = 0xF1, // SBC ($nn),Y Indirect,Y

        CMP_IMM = 0xC9, // CMP #$nn Immediate
        CMP_ZP = 0xC5,  // CMP $nn Zero Page
        CMP_ZPX = 0xD5, // CMP $nn,X Zero Page,X
        CMP_ABS = 0xCD, // CMP $nnnn Absolute
        CMP_ABSX = 0xDD, // CMP $nnnn,X Absolute,X
        CMP_ABSY = 0xD9, // CMP $nnnn,Y Absolute,Y
        CMP_INDX = 0xC1, // CMP ($nn,X) Indirect,X
        CMP_INDY = 0xD1, // CMP ($nn),Y Indirect,Y

        CPX_IMM = 0xE0, // CPX #$nn Immediate
        CPX_ZP = 0xE4,  // CPX $nn Zero Page
        CPX_ABS = 0xEC, // CPX $nnnn Absolute

        CPY_IMM = 0xC0, // CPY #$nn Immediate
        CPY_ZP = 0xC4,  // CPY $nn Zero Page
        CPY_ABS = 0xCC, // CPY $nnnn Absolute

        // Increments & Decrements
        INC_ZP = 0xE6,  // INC $nn Zero Page
        INC_ZPX = 0xF6, // INC $nn,X Zero Page,X
        INC_ABS = 0xEE, // INC $nnnn Absolute
        INC_ABSX = 0xFE, // INC $nnnn,X Absolute,X
        DEC_ZP = 0xC6,  // DEC $nn Zero Page
        DEC_ZPX = 0xD6, // DEC $nn,X Zero Page,X
        DEC_ABS = 0xCE, // DEC $nnnn Absolute
        DEC_ABSX = 0xDE, // DEC $nnnn,X Absolute,X

        // Shifts
        ASL_ACC = 0x0A, // ASL Accumulator
        ASL_ZP = 0x06,  // ASL $nn Zero Page
        ASL_ZPX = 0x16, // ASL $nn,X Zero Page,X
        ASL_ABS = 0x0E, // ASL $nnnn Absolute
        ASL_ABSX = 0x1E, // ASL $nnnn,X Absolute,X

        LSR_ACC = 0x4A, // LSR Accumulator
        LSR_ZP = 0x46,  // LSR $nn Zero Page
        LSR_ZPX = 0x56, // LSR $nn,X Zero Page,X
        LSR_ABS = 0x4E, // LSR $nnnn Absolute
        LSR_ABSX = 0x5E, // LSR $nnnn,X Absolute,X

        ROL_ACC = 0x2A, // ROL Accumulator
        ROL_ZP = 0x26,  // ROL $nn Zero Page
        ROL_ZPX = 0x36, // ROL $nn,X Zero Page,X
        ROL_ABS = 0x2E, // ROL $nnnn Absolute
        ROL_ABSX = 0x3E, // ROL $nnnn,X Absolute,X

        ROR_ACC = 0x6A, // ROR Accumulator
        ROR_ZP = 0x66,  // ROR $nn Zero Page
        ROR_ZPX = 0x76, // ROR $nn,X Zero Page,X
        ROR_ABS = 0x6E, // ROR $nnnn Absolute
        ROR_ABSX = 0x7E, // ROR $nnnn,X Absolute,X

        // Jumps & Calls
        JMP_ABS = 0x4C, // JMP $nnnn Absolute
        JMP_IND = 0x6C, // JMP ($nnnn) Indirect
        JSR = 0x20,     // JSR $nnnn Jump to Subroutine
        RTS = 0x60,     // Return from Subroutine

        // Branches
        BPL = 0x10, // Branch on Plus
        BMI = 0x30, // Branch on Minus
        BVC = 0x50, // Branch on Overflow Clear
        BVS = 0x70, // Branch on Overflow Set
        BCC = 0x90, // Branch on Carry Clear
        BCS = 0xB0, // Branch on Carry Set
        BNE = 0xD0, // Branch on Not Equal
        BEQ = 0xF0, // Branch on Equal

        // Status Flag Changes
        CLC = 0x18, // Clear Carry
        SEC = 0x38, // Set Carry
        CLI = 0x58, // Clear Interrupt Disable
        SEI = 0x78, // Set Interrupt Disable
        CLV = 0xB8, // Clear Overflow
        CLD = 0xD8, // Clear Decimal
        SED = 0xF8, // Set Decimal

        // System Functions
        BRK = 0x00, // Force Interrupt
        NOP = 0xEA, // No Operation
        RTI = 0x40, // Return from Interrupt
    };

    enum class AddressingMode : uint8_t 
    {
        IMP, // Implied
        ACC, // Accumulator
        IMM, // Immediate
        ZP,  // Zero Page
        ZPX, // Zero Page,X
        ZPY, // Zero Page,Y
        ABS, // Absolute
        ABSX,// Absolute,X
        ABSY,// Absolute,Y
        IND,// Indirect
        INDX,// Indirect,X
        INDY,// Indirect,Y
        REL, // Relative
    };

    struct OpcodeInfo 
    {
        Opcode opcode; // The opcode value
        AddressingMode mode; // The addressing mode used by the opcode
        size_t cycles; // Number of cycles required to execute the opcode
    };   
    
    static std::unordered_map<Opcode, OpcodeInfo> OpcodeMap
    {
        // Load/Store Operations
        {Opcode::LDA_IMM, { Opcode::LDA_IMM, AddressingMode::IMM, 2}},
        {Opcode::LDA_ZP, { Opcode::LDA_ZP, AddressingMode::ZP, 3}},
        {Opcode::LDA_ZPX, { Opcode::LDA_ZPX, AddressingMode::ZPX, 4}},
        {Opcode::LDA_ABS, { Opcode::LDA_ABS, AddressingMode::ABS, 4}},
        {Opcode::LDA_ABSX, { Opcode::LDA_ABSX, AddressingMode::ABSX, 4}},
        {Opcode::LDA_ABSY, { Opcode::LDA_ABSY, AddressingMode::ABSY, 4}},
        {Opcode::LDA_INDX, { Opcode::LDA_INDX, AddressingMode::INDX, 6}},
        {Opcode::LDA_INDY, { Opcode::LDA_INDY, AddressingMode::INDY, 5}},
        {Opcode::LDX_IMM, { Opcode::LDX_IMM, AddressingMode::IMM, 2}},
        {Opcode::LDX_ZP, { Opcode::LDX_ZP, AddressingMode::ZP, 3}},
        {Opcode::LDX_ZPY, { Opcode::LDX_ZPY, AddressingMode::ZPY, 4}},
        {Opcode::LDX_ABS, { Opcode::LDX_ABS, AddressingMode::ABS, 4}},
        {Opcode::LDX_ABSY, { Opcode::LDX_ABSY, AddressingMode::ABSY, 4}},
        {Opcode::LDY_IMM, { Opcode::LDY_IMM, AddressingMode::IMM, 2}},
        {Opcode::LDY_ZP, { Opcode::LDY_ZP, AddressingMode::ZP, 3}},
        {Opcode::LDY_ZPX, { Opcode::LDY_ZPX, AddressingMode::ZPX, 4}},
        {Opcode::LDY_ABS, { Opcode::LDY_ABS, AddressingMode::ABS, 4}},
        {Opcode::LDY_ABSX, { Opcode::LDY_ABSX, AddressingMode::ABSX, 4}},

        {Opcode::STA_ZP, { Opcode::STA_ZP, AddressingMode::ZP, 3}},
        {Opcode::STA_ZPX, { Opcode::STA_ZPX, AddressingMode::ZP, 4}},
        {Opcode::STA_ABS, { Opcode::STA_ABS, AddressingMode::ABS, 4}},
        {Opcode::STA_ABSX, { Opcode::STA_ABSX, AddressingMode::ABS, 5}},        
        {Opcode::STA_ABSY, { Opcode::STA_ABSY, AddressingMode::ABS, 5}},
        {Opcode::STA_INDX, { Opcode::STA_INDX, AddressingMode::ABS, 6}},
        {Opcode::STA_INDY, { Opcode::STA_INDY, AddressingMode::ABS, 6}},
        {Opcode::STX_ZP, { Opcode::STX_ZP, AddressingMode::ZP, 3}},
        {Opcode::STX_ZPY, { Opcode::STX_ZPY, AddressingMode::ZPY, 4}},
        {Opcode::STX_ABS, { Opcode::STX_ABS, AddressingMode::ABS, 4}},
        {Opcode::STY_ZP, { Opcode::STY_ZP, AddressingMode::ZP, 3}},
        {Opcode::STY_ZPX, { Opcode::STY_ZPX, AddressingMode::ZPX, 4}},
        {Opcode::STY_ABS, { Opcode::STY_ABS, AddressingMode::ABS, 4}},

        // Register Transfers
        {Opcode::TAX, { Opcode::TAX, AddressingMode::IMP, 2}},
        {Opcode::TXA, { Opcode::TXA, AddressingMode::IMP, 2}},
        {Opcode::DEX, { Opcode::DEX, AddressingMode::IMP, 2}},
        {Opcode::INX, { Opcode::INX, AddressingMode::IMP, 2}},
        {Opcode::TAY, { Opcode::TAY, AddressingMode::IMP, 2}},
        {Opcode::TYA, { Opcode::TYA, AddressingMode::IMP, 2}},
        {Opcode::DEY, { Opcode::DEY, AddressingMode::IMP, 2}},
        {Opcode::INY, { Opcode::INY, AddressingMode::IMP, 2}},

        // Stack Operations
        {Opcode::TSX, { Opcode::TSX, AddressingMode::IMP, 2}},
        {Opcode::TXS, { Opcode::TXS, AddressingMode::IMP, 2}},
        {Opcode::PHA, { Opcode::PHA, AddressingMode::IMP, 3}},
        {Opcode::PLA, { Opcode::PLA, AddressingMode::IMP, 4}},
        {Opcode::PHP, { Opcode::PHP, AddressingMode::IMP, 3}},
        {Opcode::PLP, { Opcode::PLP, AddressingMode::IMP, 4}},

        // Jump & Calls
        {Opcode::JMP_ABS, { Opcode::JMP_ABS, AddressingMode::ABS, 3}},
        {Opcode::JMP_IND, { Opcode::JMP_IND, AddressingMode::IND, 5}},
        {Opcode::JSR, { Opcode::JSR, AddressingMode::ABS, 6}},
        {Opcode::RTS, { Opcode::RTS, AddressingMode::IMP, 6}},


        
        {Opcode::NOP, {Opcode::NOP, AddressingMode::IMP, 2}}, // NOP is a no-operation instruction
        {Opcode::BRK, {Opcode::BRK, AddressingMode::IMP, 7}}, // BRK is a break instruction
        {Opcode::RTI, {Opcode::RTI, AddressingMode::IMP, 6}}, // RTI is a return from interrupt instruction

    };
}