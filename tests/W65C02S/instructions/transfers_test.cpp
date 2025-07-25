// Test suite for LDA opcode
#include "cpu_instruction_test.h"
#include "devices/SRAM62256/SRAM62256.h"
#include <cstdint>
#include <sys/types.h>

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, TAX_TransferValue) 
{
    auto opcode = Opcode::TAX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setAccumulator(0x42);
    cpu->setXRegister(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getXRegister(), 0x42);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, TAX_TransferNegativeValue) 
{
    auto opcode = Opcode::TAX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setAccumulator(0x82);
    cpu->setXRegister(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getXRegister(), 0x82);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);    
}

TEST_F(CPUInstructionTest, TAX_TransferZeroValue) 
{
    auto opcode = Opcode::TAX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setAccumulator(0x00);
    cpu->setXRegister(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getXRegister(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, TXA_TransferValue) 
{
    auto opcode = Opcode::TXA;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setXRegister(0x42);
    cpu->setAccumulator(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getAccumulator(), 0x42);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, TXA_TransferNegativeValue) 
{
    auto opcode = Opcode::TXA;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setXRegister(0x82);
    cpu->setAccumulator(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getAccumulator(), 0x82);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);    
}

TEST_F(CPUInstructionTest, TXA_TransferZeroValue) 
{
    auto opcode = Opcode::TXA;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setXRegister(0x00);
    cpu->setAccumulator(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getAccumulator(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, TAY_TransferValue) 
{
    auto opcode = Opcode::TAY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setAccumulator(0x42);
    cpu->setYRegister(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getYRegister(), 0x42);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, TAY_TransferNegativeValue) 
{
    auto opcode = Opcode::TAY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setAccumulator(0x82);
    cpu->setYRegister(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getYRegister(), 0x82);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);    
}

TEST_F(CPUInstructionTest, TAY_TransferZeroValue) 
{
    auto opcode = Opcode::TAY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setAccumulator(0x00);
    cpu->setYRegister(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getYRegister(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, TYA_TransferValue) 
{
    auto opcode = Opcode::TYA;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setYRegister(0x42);
    cpu->setAccumulator(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getAccumulator(), 0x42);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, TYA_TransferNegativeValue) 
{
    auto opcode = Opcode::TYA;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setYRegister(0x82);
    cpu->setAccumulator(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getAccumulator(), 0x82);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);    
}

TEST_F(CPUInstructionTest, TYA_TransferZeroValue) 
{
    auto opcode = Opcode::TYA;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setYRegister(0x00);
    cpu->setAccumulator(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getAccumulator(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, TSX_TransferValue) 
{
    auto opcode = Opcode::TSX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setStackPointer(0x42);
    cpu->setXRegister(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getXRegister(), 0x42);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, TSX_TransferNegativeValue) 
{
    auto opcode = Opcode::TSX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setStackPointer(0x82);
    cpu->setXRegister(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getXRegister(), 0x82);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);    
}

TEST_F(CPUInstructionTest, TSX_TransferZeroValue) 
{
    auto opcode = Opcode::TSX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setStackPointer(0x00);
    cpu->setXRegister(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getXRegister(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, TXS_TransferValue) 
{
    auto opcode = Opcode::TXS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setXRegister(0x42);
    cpu->setStackPointer(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getStackPointer(), 0x42);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);    
}

TEST_F(CPUInstructionTest, TXS_TransferNegativeValue) 
{
    auto opcode = Opcode::TXS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    auto statusBefore = cpu->getStatus();

    cpu->setXRegister(0x82);
    cpu->setStackPointer(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getStackPointer(), 0x82);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); 
}

TEST_F(CPUInstructionTest, TXS_TransferZeroValue) 
{
    auto opcode = Opcode::TXS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    auto statusBefore = cpu->getStatus();

    cpu->setXRegister(0x00);
    cpu->setStackPointer(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getStackPointer(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore);
}

TEST_F(CPUInstructionTest, INX_IncrementXValue) 
{
    auto opcode = Opcode::INX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setXRegister(0x03);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getXRegister(), 0x04);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);  
}

TEST_F(CPUInstructionTest, INX_IncrementXToNegativeValue) 
{
    auto opcode = Opcode::INX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setXRegister(0x7F);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getXRegister(), 0x80);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);  
}

TEST_F(CPUInstructionTest, INX_IncrementXToZeroValue) 
{
    auto opcode = Opcode::INX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setXRegister(0xFF);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getXRegister(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);  
}

TEST_F(CPUInstructionTest, INY_IncrementXValue) 
{
    auto opcode = Opcode::INY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setYRegister(0x03);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getYRegister(), 0x04);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);  
}

TEST_F(CPUInstructionTest, INY_IncrementXToNegativeValue) 
{
    auto opcode = Opcode::INY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setYRegister(0x7F);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getYRegister(), 0x80);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);  
}

TEST_F(CPUInstructionTest, INY_IncrementXToZeroValue) 
{
    auto opcode = Opcode::INY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setYRegister(0xFF);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getYRegister(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);  
}

TEST_F(CPUInstructionTest, DEX_DecrementXValue) 
{
    auto opcode = Opcode::DEX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setXRegister(0x03);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getXRegister(), 0x02);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);  
}

TEST_F(CPUInstructionTest, DEX_DecrementXToNegativeValue) 
{
    auto opcode = Opcode::DEX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setXRegister(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getXRegister(), 0xFF);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);  
}

TEST_F(CPUInstructionTest, DEX_DecrementXToZeroValue) 
{
    auto opcode = Opcode::DEX;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setXRegister(0x01);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getXRegister(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);  
}

TEST_F(CPUInstructionTest, DEY_DecrementXValue) 
{
    auto opcode = Opcode::DEY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setYRegister(0x03);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getYRegister(), 0x02);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);  
}

TEST_F(CPUInstructionTest, DEY_DecrementXToNegativeValue) 
{
    auto opcode = Opcode::DEY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setYRegister(0x00);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getYRegister(), 0xFF);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, 0);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, devices::STATUS_NEGATIVE);  
}

TEST_F(CPUInstructionTest, DEY_DecrementXToZeroValue) 
{
    auto opcode = Opcode::DEY;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());

    cpu->setYRegister(0x01);
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }

    EXPECT_EQ(cpu->getYRegister(), 0x00);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFD); 
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status & devices::STATUS_ZERO, devices::STATUS_ZERO);
    EXPECT_EQ(status & devices::STATUS_NEGATIVE, 0);  
}