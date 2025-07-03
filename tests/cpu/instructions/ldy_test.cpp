// Test suite for LDY opcode
#include "cpu_instruction_test.h"

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, LDY_IMM_LoadsImmediateValue) {
    memory[0x0000] = static_cast<uint8_t>(Opcode::LDY_IMM);
    memory[0x0001] = 0x77;
    cpu.pc() = 0x0000;
    cpu.runNextInstruction();
    EXPECT_EQ(cpu.y(), 0x77);
}
// Add more LDY addressing mode tests as you implement them
