// Test suite for ORA (Logical OR) opcode
#include "cpu_instruction_test.h"
#include "devices/SRAM62256/SRAM62256.h"
#include <cstdint>

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, ORA_IMM_OrsImmediateValue) 
{
    auto opcode = Opcode::ORA_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x0F
    cpu->setAccumulator(0x0F);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xF0; // OR with 0xF0
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0xFF); // 0x0F | 0xF0 = 0xFF
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_NE(status & devices::STATUS_NEGATIVE, 0); // Negative flag should be set (0xFF has bit 7 set)
}

TEST_F(CPUInstructionTest, ORA_IMM_SetsZeroFlag) 
{
    auto opcode = Opcode::ORA_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x00
    cpu->setAccumulator(0x00);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x00; // OR with 0x00
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x00); // 0x00 | 0x00 = 0x00
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_NE(status & devices::STATUS_ZERO, 0); // Zero flag should be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, ORA_IMM_PreservesExistingBits) 
{
    auto opcode = Opcode::ORA_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x55
    cpu->setAccumulator(0x55);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xAA; // OR with 0xAA
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0xFF); // 0x55 | 0xAA = 0xFF
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_NE(status & devices::STATUS_NEGATIVE, 0); // Negative flag should be set
}

TEST_F(CPUInstructionTest, ORA_IMM_SetsNegativeFlag) 
{
    auto opcode = Opcode::ORA_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x00
    cpu->setAccumulator(0x00);
    
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x80; // OR with 0x80 (negative bit)
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x80); // 0x00 | 0x80 = 0x80
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_NE(status & devices::STATUS_NEGATIVE, 0); // Negative flag should be set
}

TEST_F(CPUInstructionTest, ORA_ZP_OrsZeroPageValue) 
{
    auto opcode = Opcode::ORA_ZP;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x11
    cpu->setAccumulator(0x11);
    
    // Set up memory: ORA $42
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x42; // Zero page address
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    auto sram = std::make_unique<devices::SRAM62256>(bus);
    bus->addSlave(sram.get());

    auto& sramData = sram->getMemory();
    sramData[0x42] = 0x22; // Value at zero page address $42

    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x33); // 0x11 | 0x22 = 0x33
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, ORA_ABS_OrsAbsoluteValue) 
{
    auto opcode = Opcode::ORA_ABS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x0F
    cpu->setAccumulator(0x0F);
    
    // Set up memory: ORA $1234
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x34; // Low byte of address
    memory[0xFFFE - MEMORY_OFFSET] = 0x12; // High byte of address
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    auto sram = std::make_unique<devices::SRAM62256>(bus);
    bus->addSlave(sram.get());
    
    auto& sramData = sram->getMemory();
    sramData[0x1234 - 0x0000] = 0x70; // Value at address $1234

    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x7F); // 0x0F | 0x70 = 0x7F
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, ORA_ZPX_OrsZeroPageXValue) 
{
    auto opcode = Opcode::ORA_ZPX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set accumulator to 0x08 and X register to 0x03
    cpu->setAccumulator(0x08);
    cpu->setXRegister(0x03);
    
    // Set up memory: ORA $40,X (effective address = $40 + $03 = $43)
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x40; // Zero page base address
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());

    auto sram = std::make_unique<devices::SRAM62256>(bus);
    bus->addSlave(sram.get());
        
    auto& sramData = sram->getMemory();
    sramData[0x43] = 0x04; // Value at zero page address $43

    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x0C); // 0x08 | 0x04 = 0x0C
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}