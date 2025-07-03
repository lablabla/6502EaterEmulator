// Test suite for AND opcode
#include "cpu_instruction_test.h"

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, AND_IMM_AndsImmediateValue) {
    cpu.a() = 0xF0;
    memory[0x0000] = static_cast<uint8_t>(Opcode::AND_IMM);
    memory[0x0001] = 0x0F;
    cpu.pc() = 0x0000;
    cpu.runNextInstruction();
    EXPECT_EQ(cpu.a(), 0x00);
}
// Add more AND addressing mode tests as you implement them
