// Test suite for ADC (Add with Carry) opcode
#include "cpu_instruction_test.h"
#include "devices/SRAM62256/SRAM62256.h"
#include <cstdint>

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, ADC_IMM_AddsImmediateValue) 
{
    auto opcode = Opcode::ADC_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x10
    cpu->setAccumulator(0x10);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x20; // Add 0x20
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x30); // 0x10 + 0x20 = 0x30
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
    EXPECT_EQ(status & devices::STATUS_CARRY, 0); // Carry flag should not be set
}

TEST_F(CPUInstructionTest, ADC_IMM_AddsWithCarryFlag) 
{
    auto opcode = Opcode::ADC_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x10 and carry flag
    cpu->setAccumulator(0x10);
    cpu->setStatus(devices::STATUS_CARRY);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x20; // Add 0x20
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x31); // 0x10 + 0x20 + 1 = 0x31
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
    EXPECT_EQ(status & devices::STATUS_CARRY, 0); // Carry flag should not be set
}

TEST_F(CPUInstructionTest, ADC_IMM_SetsCarryFlag) 
{
    auto opcode = Opcode::ADC_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0xFF to cause overflow
    cpu->setAccumulator(0xFF);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x01; // Add 0x01
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x00); // 0xFF + 0x01 = 0x100 (wraps to 0x00)
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_NE(status & devices::STATUS_ZERO, 0); // Zero flag should be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
    EXPECT_NE(status & devices::STATUS_CARRY, 0); // Carry flag should be set
}

TEST_F(CPUInstructionTest, ADC_IMM_SetsNegativeFlag) 
{
    auto opcode = Opcode::ADC_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x70
    cpu->setAccumulator(0x70);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x20; // Add 0x20
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x90); // 0x70 + 0x20 = 0x90
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_NE(status & devices::STATUS_NEGATIVE, 0); // Negative flag should be set
    EXPECT_EQ(status & devices::STATUS_CARRY, 0); // Carry flag should not be set
}

TEST_F(CPUInstructionTest, ADC_ZP_AddsZeroPageValue) 
{
    auto opcode = Opcode::ADC_ZP;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x15
    cpu->setAccumulator(0x15);
    
    // Set up memory: ADC $42
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x42; // Zero page address
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    auto sram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(sram.get());
    
    auto& sramData = sram->getMemory();
    sramData[0x42] = 0x25; // Value at zero page address $42
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x3A); // 0x15 + 0x25 = 0x3A
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
    EXPECT_EQ(status & devices::STATUS_CARRY, 0); // Carry flag should not be set
}

TEST_F(CPUInstructionTest, ADC_ABS_AddsAbsoluteValue) 
{
    auto opcode = Opcode::ADC_ABS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x30
    cpu->setAccumulator(0x30);
    
    // Set up memory: ADC $1234
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x34; // Low byte of address
    memory[0xFFFE - MEMORY_OFFSET] = 0x12; // High byte of address
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    auto sram = std::make_unique<devices::SRAM62256>( bus);
    bus.addSlave(sram.get());
    
    auto& sramData = sram->getMemory();
    sramData[0x1234 - 0x0000] = 0x40; // Value at address $1234
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x70); // 0x30 + 0x40 = 0x70
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
    EXPECT_EQ(status & devices::STATUS_CARRY, 0); // Carry flag should not be set
}