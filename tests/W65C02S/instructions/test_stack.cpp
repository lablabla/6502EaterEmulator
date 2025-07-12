// Test suite for LDA opcode
#include "cpu_instruction_test.h"
#include "devices/SRAM62256/SRAM62256.h"
#include "devices/W65C02S/W65C02S.h"
#include <cstdint>
#include <sys/types.h>

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, PHA_PushValue) 
{
    auto opcode = Opcode::PHA;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    cpu->setAccumulator(0x42);
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(ram.get());
    cpu->setStackPointer(0xFF); // Set stack pointer to the top of the stack
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    auto ramMemory = ram->getMemory();
    EXPECT_EQ(ramMemory[0x01FF], 0x42);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFC + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}

TEST_F(CPUInstructionTest, PLA_PullValue) 
{
    auto opcode = Opcode::PLA;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    cpu->setAccumulator(0x00);
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(ram.get());
    cpu->setStackPointer(0xFE); // Set stack pointer to the top of the stack - 1
    ram->getMemory()[0x01FF] = 0x42; 
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getAccumulator(), 0x42);
    EXPECT_EQ(cpu->getStackPointer(), 0xFF);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFC + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}

TEST_F(CPUInstructionTest, PHP_PushStatus) 
{
    auto opcode = Opcode::PHP;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(ram.get());
    cpu->setStackPointer(0xFF); // Set stack pointer to the top of the stack
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    auto ramMemory = ram->getMemory();
    EXPECT_EQ(ramMemory[0x01FF], statusBefore);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFC + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}

TEST_F(CPUInstructionTest, PLP_PullStatus) 
{
    auto opcode = Opcode::PLP;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    cpu->setStatus(0x00);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(ram.get());
    cpu->setStackPointer(0xFE); // Set stack pointer to the top of the stack - 1
    ram->getMemory()[0x01FF] = devices::STATUS_NEGATIVE | devices::STATUS_ZERO;
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getStatus(), devices::STATUS_NEGATIVE | devices::STATUS_ZERO);
    EXPECT_EQ(cpu->getStackPointer(), 0xFF);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFC + 1);
}