// Test suite for LDA opcode
#include "cpu_instruction_test.h"
#include <cstdint>
#include <sys/types.h>

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, STA_ABS_StoreValue) 
{
    auto opcode = Opcode::STA_ABS;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    memory[0xFFFC - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0xFFFD - MEMORY_OFFSET] = 0x37;
    memory[0xFFFE - MEMORY_OFFSET] = 0x80;
    memory[0x8037 - MEMORY_OFFSET] = 0x8F; // Absolute address 0x8037 contains value 0x8F, should be overwritten
    cpu->setAccumulator(0x42);
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    auto romMemory = rom->getMemory();
    EXPECT_EQ(romMemory[0x8037 - MEMORY_OFFSET], 0x42);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}