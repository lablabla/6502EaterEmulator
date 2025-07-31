// Test suite for LDA opcode
#include "cpu_instruction_test.h"
#include "devices/SRAM62256/SRAM62256.h"
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
    memory[0xFFFE - MEMORY_OFFSET] = 0x20;
    cpu->setAccumulator(0x42);
    uint8_t statusBefore = cpu->getStatus();
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus->addSlave(rom.get());
    auto ram = std::make_unique<devices::SRAM62256>(bus);
    bus->addSlave(ram.get());
    
    for (int i = 0; i < cycles; ++i) 
    {
        cpu->onClockStateChange(core::LOW);
        cpu->onClockStateChange(core::HIGH);
    }
    
    auto ramMemory = ram->getMemory();
    EXPECT_EQ(ramMemory[0x2037], 0x42);
    EXPECT_EQ(cpu->getProgramCounter(), 0xFFFE + 1);
    uint8_t status = cpu->getStatus();
    EXPECT_EQ(status, statusBefore); // Status should remain unchanged
}