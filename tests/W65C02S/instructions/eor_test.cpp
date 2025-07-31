// Test suite for EOR (Exclusive OR) opcode
#include "cpu_instruction_test.h"
#include "devices/SRAM62256/SRAM62256.h"
#include <cstdint>

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, EOR_IMM_XorsImmediateValue) 
{
    auto opcode = Opcode::EOR_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0xF0
    cpu->setAccumulator(0xF0);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x0F; // XOR with 0x0F
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0xFF); // 0xF0 ^ 0x0F = 0xFF
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_NE(status & devices::STATUS_NEGATIVE, 0); // Negative flag should be set (0xFF has bit 7 set)
}

TEST_F(CPUInstructionTest, EOR_IMM_SetsZeroFlag) 
{
    auto opcode = Opcode::EOR_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0xAA
    cpu->setAccumulator(0xAA);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xAA; // XOR with same value
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x00); // 0xAA ^ 0xAA = 0x00
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_NE(status & devices::STATUS_ZERO, 0); // Zero flag should be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, EOR_IMM_ClearsNegativeFlag) 
{
    auto opcode = Opcode::EOR_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x80 (negative)
    cpu->setAccumulator(0x80);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x81; // XOR with 0x81
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x01); // 0x80 ^ 0x81 = 0x01
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, EOR_ZP_XorsZeroPageValue) 
{
    auto opcode = Opcode::EOR_ZP;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x55
    cpu->setAccumulator(0x55);
    
    // Set up memory: EOR $42
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x42; // Zero page address
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    auto sram = std::make_unique<devices::SRAM62256>(bus);
    bus->addSlave(sram.get());
    
    auto& sramData = sram->getMemory();
    sramData[0x42] = 0x33; // Value at zero page address $42

    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x66); // 0x55 ^ 0x33 = 0x66
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, EOR_ABS_XorsAbsoluteValue) 
{
    auto opcode = Opcode::EOR_ABS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x12
    cpu->setAccumulator(0x12);
    
    // Set up memory: EOR $1234
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x34; // Low byte of address
    memory[0xFFFE - MEMORY_OFFSET] = 0x12; // High byte of address
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    auto sram = std::make_unique<devices::SRAM62256>(bus);
    bus->addSlave(sram.get());
    
    auto& sramData = sram->getMemory();
    sramData[0x1234 - 0x0000] = 0x34; // Value at address $1234

    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x26); // 0x12 ^ 0x34 = 0x26
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, EOR_ZPX_XorsZeroPageXValue) 
{
    auto opcode = Opcode::EOR_ZPX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0xFF and X register to 0x05
    cpu->setAccumulator(0xFF);
    cpu->setXRegister(0x05);
    
    // Set up memory: EOR $40,X (effective address = $40 + $05 = $45)
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x40; // Zero page base address
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    auto sram = std::make_unique<devices::SRAM62256>( bus);
    bus->addSlave(sram.get());
    
    auto& sramData = sram->getMemory();
    sramData[0x45] = 0x0F; // Value at zero page address $45

    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0xF0); // 0xFF ^ 0x0F = 0xF0
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_NE(status & devices::STATUS_NEGATIVE, 0); // Negative flag should be set
}