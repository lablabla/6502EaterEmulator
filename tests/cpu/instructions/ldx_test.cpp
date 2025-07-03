// Test suite for LDX opcode
#include "cpu_instruction_test.h"

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, LDX_IMM_LoadsImmediateValue) {
    memory[0x0000] = static_cast<uint8_t>(Opcode::LDX_IMM);
    memory[0x0001] = 0x24;
    cpu.pc() = 0x0000;
    cpu.runNextInstruction();
    EXPECT_EQ(cpu.x(), 0x24);
}
// Add more LDX addressing mode tests as you implement them
