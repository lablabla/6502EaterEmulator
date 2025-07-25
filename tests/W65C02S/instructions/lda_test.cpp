// Test suite for LDA opcode
#include "cpu_instruction_test.h"
#include "devices/SRAM62256/SRAM62256.h"
#include <cstdint>
#include <sys/types.h>

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, LDA_IMM_LoadsImmediateValue) 
{
    auto opcode = Opcode::LDA_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x42;
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x42);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_IMM_LoadsImmediateNegativeValue) 
{
    auto opcode = Opcode::LDA_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x8E;
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x8E);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE); // Negative flag should be set
}

TEST_F(CPUInstructionTest, LDA_IMM_LoadsImmediateZeroValue) 
{
    auto opcode = Opcode::LDA_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x00;
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO); // Zero flag should be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_ABS_LoadsAbsoluteValue) 
{
    auto opcode = Opcode::LDA_ABS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    memory[0xFFFE - MEMORY_OFFSET] = 0x80;
    memory[0x8037 - MEMORY_OFFSET] = 0x11;
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x11);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_ABS_LoadsAbsoluteNegativeValue) 
{
    auto opcode = Opcode::LDA_ABS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    memory[0xFFFE - MEMORY_OFFSET] = 0x80;
    memory[0x8037 - MEMORY_OFFSET] = 0x8F; 
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x8F);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE); // Negative flag should be set
}

TEST_F(CPUInstructionTest, LDA_ABS_LoadsAbsoluteZeroValue) 
{
    auto opcode = Opcode::LDA_ABS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    memory[0xFFFE - MEMORY_OFFSET] = 0x80;
    memory[0x8037 - MEMORY_OFFSET] = 0x00;
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO); // Zero flag should be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_ABSX_LoadsAbsoluteXValue) 
{
    auto opcode = Opcode::LDA_ABSX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    memory[0xFFFE - MEMORY_OFFSET] = 0x80;
    memory[0x803B - MEMORY_OFFSET] = 0x11;
    cpu->setXRegister(0x04);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x11);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_ABSX_LoadsAbsoluteNegativeXValue) 
{
    auto opcode = Opcode::LDA_ABSX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    memory[0xFFFE - MEMORY_OFFSET] = 0x80;
    memory[0x803B - MEMORY_OFFSET] = 0x8F;
    cpu->setXRegister(0x04);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x8F);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE); // Negative flag should  be set
}

TEST_F(CPUInstructionTest, LDA_ABSX_LoadsAbsoluteXZeroValue) 
{
    auto opcode = Opcode::LDA_ABSX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    memory[0xFFFE - MEMORY_OFFSET] = 0x80;
    memory[0x803B - MEMORY_OFFSET] = 0x0;
    cpu->setXRegister(0x04);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x0);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_ABSY_LoadsAbsoluteYValue) 
{
    auto opcode = Opcode::LDA_ABSY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    memory[0xFFFE - MEMORY_OFFSET] = 0x80;
    memory[0x803B - MEMORY_OFFSET] = 0x11; // Absolute address 0x8037 contains value 0x11
    cpu->setYRegister(0x04);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x11);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_ABSX_LoadsAbsoluteYNegativeValue) 
{
    auto opcode = Opcode::LDA_ABSY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    memory[0xFFFE - MEMORY_OFFSET] = 0x80;
    memory[0x803B - MEMORY_OFFSET] = 0x8F;
    cpu->setYRegister(0x04);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x8F);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE); // Negative flag should  be set
}

TEST_F(CPUInstructionTest, LDA_ABSX_LoadsAbsoluteYZeroValue) 
{
    auto opcode = Opcode::LDA_ABSY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    memory[0xFFFE - MEMORY_OFFSET] = 0x80;
    memory[0x803B - MEMORY_OFFSET] = 0x0;
    cpu->setYRegister(0x04);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x0);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_ZP_LoadsZeroPageValue) 
{
    auto opcode = Opcode::LDA_ZP;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    
    bus->addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    ram->getMemory()[0x0037] = 0x11; // Zero page address 0x0037 contains value 0x11
    bus->addSlave(ram.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x11);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_ZP_LoadsZeroPageNegativeValue) 
{
    auto opcode = Opcode::LDA_ZP;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    
    bus->addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    ram->getMemory()[0x0037] = 0x81; // Zero page address 0x0081 contains value 0x11
    bus->addSlave(ram.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x81);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_ZP_LoadsZeroPageZeroValue) 
{
    auto opcode = Opcode::LDA_ZP;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    
    bus->addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    ram->getMemory()[0x0037] = 0x00; // Zero page address 0x0037 contains value 0x11
    bus->addSlave(ram.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_ZPX_LoadsZeroPageXValue) 
{
    auto opcode = Opcode::LDA_ZPX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x80;
    cpu->setXRegister(0x0F);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    
    bus->addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    ram->getMemory()[0x008F] = 0x11; // Zero page address 0x0037 contains value 0x11
    bus->addSlave(ram.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x11);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_ZPX_LoadsZeroPageXNegativeValue) 
{
    auto opcode = Opcode::LDA_ZPX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x80;
    cpu->setXRegister(0x0F);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    
    bus->addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    ram->getMemory()[0x008F] = 0x84; // Zero page address 0x0037 contains value 0x11
    bus->addSlave(ram.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x84);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_ZPX_LoadsZeroPageXZeroValue) 
{
    auto opcode = Opcode::LDA_ZPX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x80;
    cpu->setXRegister(0x0F);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    
    bus->addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    ram->getMemory()[0x008F] = 0x00;
    bus->addSlave(ram.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}

TEST_F(CPUInstructionTest, LDA_ZPX_LoadsZeroPageXOverflowValue) 
{
    auto opcode = Opcode::LDA_ZPX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x80;
    cpu->setXRegister(0xFF);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    
    bus->addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    ram->getMemory()[0x007F] = 0x42;
    bus->addSlave(ram.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    EXPECT_EQ(cpu->getAccumulator(), 0x42);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0); // Zero flag should not be set
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0); // Negative flag should not be set
}