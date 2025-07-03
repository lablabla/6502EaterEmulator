// Test suite for JMP opcode
#include "cpu_instruction_test.h"

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, JMP_ABS_JumpsToAddress) {
    memory[0x0000] = static_cast<uint8_t>(Opcode::JMP_ABS);
    memory[0x0001] = 0x00;
    memory[0x0002] = 0x80;
    cpu.pc() = 0x0000;
    cpu.runNextInstruction();
    EXPECT_EQ(cpu.pc(), 0x8000);
}
// Add more JMP addressing mode tests as you implement them
