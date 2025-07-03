// Test suite for SBC opcode
#include "cpu_instruction_test.h"

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, SBC_IMM_SubtractsImmediateValue) {
    cpu.a() = 0x30;
    memory[0x0000] = static_cast<uint8_t>(Opcode::SBC_IMM);
    memory[0x0001] = 0x10;
    cpu.pc() = 0x0000;
    cpu.runNextInstruction();
    EXPECT_EQ(cpu.a(), 0x20);
}
// Add more SBC addressing mode tests as you implement them
