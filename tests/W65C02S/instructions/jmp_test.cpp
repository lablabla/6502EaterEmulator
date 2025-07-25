// Test suite for LDA opcode
#include "cpu_instruction_test.h"
#include "devices/SRAM62256/SRAM62256.h"
#include <cstdint>
#include <sys/types.h>

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, JMP_ABS) 
{
    auto opcode = Opcode::JMP_ABS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    memory[0xFFFE - MEMORY_OFFSET] = 0x80;
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getProgramCounter(), 0x8037);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}

TEST_F(CPUInstructionTest, JSR) 
{
    auto opcode = Opcode::JSR;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    cpu->setResetStage(0); // Not enough room to start from reset vector, perform full reset
    auto cycles = it->second.cycles + 2; // + 2 for the reset stages.
    memory[0xFFFC - MEMORY_OFFSET] = 0x05;
    memory[0xFFFD - MEMORY_OFFSET] = 0xFF;

    memory[0xFF05 - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFF06 - MEMORY_OFFSET] = 0x0A;
    memory[0xFF07 - MEMORY_OFFSET] = 0xFF;

    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(ram.get());

    cpu->setStackPointer(0xFF);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    auto ramMemory = ram->getMemory();
    EXPECT_EQ(cpu->getProgramCounter(), 0xFF0A);
    EXPECT_EQ(cpu->getStackPointer(), 0xFF - 2);
    EXPECT_EQ(ramMemory[0x01FF], 0xFF);
    EXPECT_EQ(ramMemory[0x01FF-1], 0x07);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}

TEST_F(CPUInstructionTest, RTS) 
{
    auto opcode = Opcode::RTS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    cpu->setResetStage(0); // Not enough room to start from reset vector, perform full reset
    auto cycles = it->second.cycles + 2; // + 2 for the reset stages.
    memory[0xFFFC - MEMORY_OFFSET] = 0x0A;
    memory[0xFFFD - MEMORY_OFFSET] = 0xFF;

    memory[0xFF0A - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);

    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus.addSlave(ram.get());

    auto& ramMemory = ram->getMemory();
    ramMemory[0x01FF] = 0xFF;
    ramMemory[0x01FF-1] = 0x07;
    cpu->setStackPointer(0xFF - 2);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getProgramCounter(), 0xFF08);
    EXPECT_EQ(cpu->getStackPointer(), 0xFF);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}

TEST_F(CPUInstructionTest, BEQ_PositiveOffsetSamePageTakeBranch) 
{
    auto opcode = Opcode::BEQ;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    cpu->setResetStage(0); // Not enough room to start from reset vector, perform full reset
    auto cycles = 4 + 2; // 3 + BEQ branch taken same page + 2 for the reset stages.
    memory[0xFFFC - MEMORY_OFFSET] = 0x05;
    memory[0xFFFD - MEMORY_OFFSET] = 0xFF;

    memory[0xFF05 - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFF06 - MEMORY_OFFSET] = 0x02;

    cpu->setStatus(devices::STATUS_ZERO);
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getProgramCounter(), 0xFF09);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}

TEST_F(CPUInstructionTest, BEQ_PositiveOffsetSamePageDontTakeBranch) 
{
    auto opcode = Opcode::BEQ;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    cpu->setResetStage(0); // Not enough room to start from reset vector, perform full reset
    auto cycles = 3 + 2; // 2 + BEQ branch not taken + 2 for the reset stages.
    memory[0xFFFC - MEMORY_OFFSET] = 0x05;
    memory[0xFFFD - MEMORY_OFFSET] = 0xFF;

    memory[0xFF05 - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFF06 - MEMORY_OFFSET] = 0x02;

    cpu->setStatus(0);
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getProgramCounter(), 0xFF07);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}

TEST_F(CPUInstructionTest, BEQ_NegativeOffsetSamePageTakeBranch) 
{
    auto opcode = Opcode::BEQ;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    cpu->setResetStage(0); // Not enough room to start from reset vector, perform full reset
    auto cycles = 4 + 2; // 3 + BEQ branch taken same page + 2 for the reset stages.
    memory[0xFFFC - MEMORY_OFFSET] = 0x05;
    memory[0xFFFD - MEMORY_OFFSET] = 0xFF;

    memory[0xFF05 - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFF06 - MEMORY_OFFSET] = -0x03;

    cpu->setStatus(devices::STATUS_ZERO);
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getProgramCounter(), 0xFF04);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}

TEST_F(CPUInstructionTest, BEQ_NegativeOffsetSamePageDontTakeBranch) 
{
    auto opcode = Opcode::BEQ;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    cpu->setResetStage(0); // Not enough room to start from reset vector, perform full reset
    auto cycles = 3 + 2; // 2 + BEQ branch not taken + 2 for the reset stages.
    memory[0xFFFC - MEMORY_OFFSET] = 0x05;
    memory[0xFFFD - MEMORY_OFFSET] = 0xFF;

    memory[0xFF05 - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFF06 - MEMORY_OFFSET] = -0x03;

    cpu->setStatus(0);
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getProgramCounter(), 0xFF07);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}

TEST_F(CPUInstructionTest, BNE_PositiveOffsetSamePageTakeBranch) 
{
    auto opcode = Opcode::BNE;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    cpu->setResetStage(0); // Not enough room to start from reset vector, perform full reset
    auto cycles = 4 + 2; // 3 + BNE branch taken same page + 2 for the reset stages.
    memory[0xFFFC - MEMORY_OFFSET] = 0x05;
    memory[0xFFFD - MEMORY_OFFSET] = 0xFF;

    memory[0xFF05 - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFF06 - MEMORY_OFFSET] = 0x02;

    cpu->setStatus(0);
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getProgramCounter(), 0xFF09);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}

TEST_F(CPUInstructionTest, BNE_PositiveOffsetSamePageDontTakeBranch) 
{
    auto opcode = Opcode::BNE;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    cpu->setResetStage(0); // Not enough room to start from reset vector, perform full reset
    auto cycles = 3 + 2; // 2 + BNE branch not taken + 2 for the reset stages.
    memory[0xFFFC - MEMORY_OFFSET] = 0x05;
    memory[0xFFFD - MEMORY_OFFSET] = 0xFF;

    memory[0xFF05 - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFF06 - MEMORY_OFFSET] = 0x02;

    cpu->setStatus(devices::STATUS_ZERO);
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getProgramCounter(), 0xFF07);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}

TEST_F(CPUInstructionTest, BNE_NegativeOffsetSamePageTakeBranch) 
{
    auto opcode = Opcode::BNE;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    cpu->setResetStage(0); // Not enough room to start from reset vector, perform full reset
    auto cycles = 4 + 2; // 3 + BNE branch taken same page + 2 for the reset stages.
    memory[0xFFFC - MEMORY_OFFSET] = 0x05;
    memory[0xFFFD - MEMORY_OFFSET] = 0xFF;

    memory[0xFF05 - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFF06 - MEMORY_OFFSET] = -0x03;

    cpu->setStatus(0);
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getProgramCounter(), 0xFF04);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}

TEST_F(CPUInstructionTest, BNE_NegativeOffsetSamePageDontTakeBranch) 
{
    auto opcode = Opcode::BNE;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    cpu->setResetStage(0); // Not enough room to start from reset vector, perform full reset
    auto cycles = 3 + 2; // 2 + BNE branch not taken + 2 for the reset stages.
    memory[0xFFFC - MEMORY_OFFSET] = 0x05;
    memory[0xFFFD - MEMORY_OFFSET] = 0xFF;

    memory[0xFF05 - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFF06 - MEMORY_OFFSET] = -0x03;

    cpu->setStatus(devices::STATUS_ZERO);
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    EXPECT_EQ(cpu->getProgramCounter(), 0xFF07);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}