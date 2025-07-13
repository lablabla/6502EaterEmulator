// Test suite for LDA opcode
#include "cpu_instruction_test.h"
#include "devices/SRAM62256/SRAM62256.h"
#include <cstdint>
#include <sys/types.h>

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, AND_IMM_Value) 
{
    auto opcode = Opcode::AND_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37; // 00110111
    cpu->setAccumulator(0x42); // 01000010
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x02);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, AND_IMM_ZeroValue) 
{
    auto opcode = Opcode::AND_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37; // 00110111
    cpu->setAccumulator(0x48); // 01001000
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, AND_IMM_NegativeValue) 
{
    auto opcode = Opcode::AND_IMM;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xB7; // 10110111
    cpu->setAccumulator(0xC8); // 11001000
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x80);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);    
}

TEST_F(CPUInstructionTest, AND_ABS_Value) 
{
    auto opcode = Opcode::AND_ABS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xF0;
    memory[0xFFFE - MEMORY_OFFSET] = 0xFF;
    memory[0xFFF0 - MEMORY_OFFSET] = 0x37; // 00110111
    cpu->setAccumulator(0x42); // 01000010
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x02);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, AND_ABS_ZeroValue) 
{
    auto opcode = Opcode::AND_ABS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xF0;
    memory[0xFFFE - MEMORY_OFFSET] = 0xFF;
    memory[0xFFF0 - MEMORY_OFFSET] = 0x37; // 00110111
    cpu->setAccumulator(0x48); // 01001000
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, AND_ABS_NegativeValue) 
{
    auto opcode = Opcode::AND_ABS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xF0;
    memory[0xFFFE - MEMORY_OFFSET] = 0xFF;
    memory[0xFFF0 - MEMORY_OFFSET] = 0xB7; // 10110111
    cpu->setAccumulator(0xC8); // 11001000
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x80);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);    
}

TEST_F(CPUInstructionTest, AND_ABSX_Value) 
{
    auto opcode = Opcode::AND_ABSX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xF0;
    memory[0xFFFE - MEMORY_OFFSET] = 0xFF;
    memory[0xFFF2 - MEMORY_OFFSET] = 0x37; // 00110111
    cpu->setXRegister(0x02);
    cpu->setAccumulator(0x42); // 01000010
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x02);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, AND_ABSX_ZeroValue) 
{
    auto opcode = Opcode::AND_ABSX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xF0;
    memory[0xFFFE - MEMORY_OFFSET] = 0xFF;
    memory[0xFFF2 - MEMORY_OFFSET] = 0x37; // 00110111
    cpu->setXRegister(0x02);
    cpu->setAccumulator(0x48); // 01001000
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, AND_ABSX_NegativeValue) 
{
    auto opcode = Opcode::AND_ABSX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xF0;
    memory[0xFFFE - MEMORY_OFFSET] = 0xFF;
    memory[0xFFF2 - MEMORY_OFFSET] = 0xB7; // 10110111
    cpu->setXRegister(0x02);
    cpu->setAccumulator(0xC8); // 11001000
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x80);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);    
}

TEST_F(CPUInstructionTest, AND_ABSY_Value) 
{
    auto opcode = Opcode::AND_ABSY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xF0;
    memory[0xFFFE - MEMORY_OFFSET] = 0xFF;
    memory[0xFFF2 - MEMORY_OFFSET] = 0x37; // 00110111
    cpu->setYRegister(0x02);
    cpu->setAccumulator(0x42); // 01000010
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x02);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, AND_ABSY_ZeroValue) 
{
    auto opcode = Opcode::AND_ABSY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xF0;
    memory[0xFFFE - MEMORY_OFFSET] = 0xFF;
    memory[0xFFF2 - MEMORY_OFFSET] = 0x37; // 00110111
    cpu->setYRegister(0x02);
    cpu->setAccumulator(0x48); // 01001000
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, AND_ABSY_NegativeValue) 
{
    auto opcode = Opcode::AND_ABSY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xF0;
    memory[0xFFFE - MEMORY_OFFSET] = 0xFF;
    memory[0xFFF2 - MEMORY_OFFSET] = 0xB7; // 10110111
    cpu->setYRegister(0x02);
    cpu->setAccumulator(0xC8); // 11001000
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x80);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);    
}

TEST_F(CPUInstructionTest, AND_ZP_Value) 
{
    auto opcode = Opcode::AND_ZP;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xF0;
    cpu->setAccumulator(0x42); // 01000010
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(ram.get());
    auto& ramMemory = ram->getMemory();
    ramMemory[0x00F0] = 0x37; // 00110111
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x02);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, AND_ZP_ZeroValue) 
{
    auto opcode = Opcode::AND_ZP;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xF0;
    cpu->setAccumulator(0x48); // 01001000
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(ram.get());
    auto& ramMemory = ram->getMemory();
    ramMemory[0x00F0] = 0x37; // 00110111
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, AND_ZP_NegativeValue) 
{
    auto opcode = Opcode::AND_ZP;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0xF0;
    cpu->setAccumulator(0xC8); // 11001000
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(ram.get());
    auto& ramMemory = ram->getMemory();
    ramMemory[0x00F0] = 0xB7; // 10110111
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x80);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);    
}

TEST_F(CPUInstructionTest, AND_ZPX_Value) 
{
    auto opcode = Opcode::AND_ZPX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x80;
    cpu->setAccumulator(0x42); // 01000010
    cpu->setXRegister(0x0F);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(ram.get());
    auto& ramMemory = ram->getMemory();
    ramMemory[0x008F] = 0x37; // 00110111
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x02);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, AND_ZPX_ZeroValue) 
{
    auto opcode = Opcode::AND_ZPX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x80;
    cpu->setAccumulator(0x48); // 01001000
    cpu->setXRegister(0x0F);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(ram.get());
    auto& ramMemory = ram->getMemory();
    ramMemory[0x008F] = 0x37; // 00110111
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, AND_ZPX_NegativeValue) 
{
    auto opcode = Opcode::AND_ZPX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x80;
    cpu->setAccumulator(0xC8); // 11001000
    cpu->setXRegister(0x0F);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(ram.get());
    auto& ramMemory = ram->getMemory();
    ramMemory[0x008F] = 0xB7; // 10110111
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x80);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);    
}

TEST_F(CPUInstructionTest, AND_ZPX_OverflowValue) 
{
    auto opcode = Opcode::AND_ZPX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x80;
    cpu->setAccumulator(0x42); // 01000010
    cpu->setXRegister(0xFF);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(ram.get());
    auto& ramMemory = ram->getMemory();
    ramMemory[0x007F] = 0x37; // 00110111
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x02);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}