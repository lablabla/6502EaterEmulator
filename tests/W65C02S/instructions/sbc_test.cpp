// Test suite for SBC (Subtract with Carry) opcode
#include "cpu_instruction_test.h"
#include "devices/SRAM62256/SRAM62256.h"
#include <cstdint>

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, SBC_IMM_SubtractsImmediateValue) 
{
    auto opcode = Opcode::SBC_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x50 and carry flag (no borrow)
    cpu->setAccumulator(0x50);
    cpu->setStatus(devices::STATUS_CARRY);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x30; // Subtract 0x30
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x20); // 0x50 - 0x30 = 0x20
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
    EXPECT_NE(status & devices::STATUS_CARRY, 0); // Carry flag should remain set (no borrow)
}

TEST_F(CPUInstructionTest, SBC_IMM_SubtractsWithBorrow) 
{
    auto opcode = Opcode::SBC_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x50, clear carry flag (borrow)
    cpu->setAccumulator(0x50);
    cpu->setStatus(0x00); // Clear carry flag
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x30; // Subtract 0x30
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x1F); // 0x50 - 0x30 - 1 = 0x1F
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
    EXPECT_NE(status & devices::STATUS_CARRY, 0); // Carry flag should be set (no borrow occurred)
}

TEST_F(CPUInstructionTest, SBC_IMM_SetsZeroFlag) 
{
    auto opcode = Opcode::SBC_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x42 and carry flag
    cpu->setAccumulator(0x42);
    cpu->setStatus(devices::STATUS_CARRY);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x42; // Subtract same value
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x00); // 0x42 - 0x42 = 0x00
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_NE(status & devices::STATUS_ZERO, 0); // Zero flag should be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
    EXPECT_NE(status & devices::STATUS_CARRY, 0); // Carry flag should be set (no borrow)
}

TEST_F(CPUInstructionTest, SBC_IMM_CausesUnderflow) 
{
    auto opcode = Opcode::SBC_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x10 and carry flag
    cpu->setAccumulator(0x10);
    cpu->setStatus(devices::STATUS_CARRY);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x20; // Subtract larger value
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0xF0); // 0x10 - 0x20 = 0xF0 (underflow)
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_NE(status & devices::STATUS_NEGATIVE, 0); // Negative flag should be set
    EXPECT_EQ(status & devices::STATUS_CARRY, 0); // Carry flag should be clear (borrow occurred)
}

TEST_F(CPUInstructionTest, SBC_IMM_SetsNegativeFlag) 
{
    auto opcode = Opcode::SBC_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x00 and carry flag
    cpu->setAccumulator(0x00);
    cpu->setStatus(devices::STATUS_CARRY);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x01; // Subtract 0x01
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0xFF); // 0x00 - 0x01 = 0xFF
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_NE(status & devices::STATUS_NEGATIVE, 0); // Negative flag should be set
    EXPECT_EQ(status & devices::STATUS_CARRY, 0); // Carry flag should be clear (borrow occurred)
}

TEST_F(CPUInstructionTest, SBC_ZP_SubtractsZeroPageValue) 
{
    auto opcode = Opcode::SBC_ZP;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x80 and carry flag
    cpu->setAccumulator(0x80);
    cpu->setStatus(devices::STATUS_CARRY);
    
    // Set up memory: SBC $42
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x42; // Zero page address
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    auto sram = std::make_unique<devices::SRAM62256>( bus);
    bus.addSlave(sram.get());
    
    auto& sramData = sram->getMemory();
    sramData[0x42] = 0x30; // Value at zero page address $42

    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x50); // 0x80 - 0x30 = 0x50
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
    EXPECT_NE(status & devices::STATUS_CARRY, 0); // Carry flag should be set (no borrow)
}

TEST_F(CPUInstructionTest, SBC_ABS_SubtractsAbsoluteValue) 
{
    auto opcode = Opcode::SBC_ABS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0xFF and carry flag
    cpu->setAccumulator(0xFF);
    cpu->setStatus(devices::STATUS_CARRY);
    
    // Set up memory: SBC $1234
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x34; // Low byte of address
    memory[0xFFFE - MEMORY_OFFSET] = 0x12; // High byte of address
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    auto sram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(sram.get());
    
    auto& sramData = sram->getMemory();
    sramData[0x1234 - 0x0000] = 0x7F; // Value at address $1234

    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x80); // 0xFF - 0x7F = 0x80
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_NE(status & devices::STATUS_NEGATIVE, 0); // Negative flag should be set
    EXPECT_NE(status & devices::STATUS_CARRY, 0); // Carry flag should be set (no borrow)
}

TEST_F(CPUInstructionTest, SBC_ZPX_SubtractsZeroPageXValue) 
{
    auto opcode = Opcode::SBC_ZPX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x60, X register to 0x02, and carry flag
    cpu->setAccumulator(0x60);
    cpu->setXRegister(0x02);
    cpu->setStatus(devices::STATUS_CARRY);
    
    // Set up memory: SBC $40,X (effective address = $40 + $02 = $42)
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x40; // Zero page base address
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    auto sram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(sram.get());
    
    auto& sramData = sram->getMemory();
    sramData[0x42] = 0x20; // Value at zero page address $42

    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x40); // 0x60 - 0x20 = 0x40
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
    EXPECT_NE(status & devices::STATUS_CARRY, 0); // Carry flag should be set (no borrow)
}