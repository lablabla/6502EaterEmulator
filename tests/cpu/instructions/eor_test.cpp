// Test suite for EOR opcode
#include "cpu_instruction_test.h"

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, EOR_IMM_EorsImmediateValue) {
    cpu.a() = 0xF0;
    memory[0x0000] = static_cast<uint8_t>(Opcode::EOR_IMM);
    memory[0x0001] = 0x0F;
    cpu.pc() = 0x0000;
    cpu.runNextInstruction();
    EXPECT_EQ(cpu.a(), 0xFF);
}
// Add more EOR addressing mode tests as you implement them
