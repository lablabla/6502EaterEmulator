// Test suite for LDA opcode
#include "cpu_instruction_test.h"

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, LDA_IMM_LoadsImmediateValue) {
    memory[0x0000] = static_cast<uint8_t>(Opcode::LDA_IMM);
    memory[0x0001] = 0x42;
    cpu.pc() = 0x0000;
    cpu.runNextInstruction();
    EXPECT_EQ(cpu.a(), 0x42);
}
// Add more LDA addressing mode tests as you implement them
