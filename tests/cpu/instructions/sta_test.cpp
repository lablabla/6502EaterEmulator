// Test suite for STA opcode
#include "cpu_instruction_test.h"

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, STA_ABS_StoresAccumulatorValue) {
    cpu.a() = 0x55;
    memory[0x0000] = static_cast<uint8_t>(Opcode::STA_ABS);
    memory[0x0001] = 0x34;
    memory[0x0002] = 0x12;
    cpu.pc() = 0x0000;
    cpu.runNextInstruction();
    EXPECT_EQ(memory[0x1234], 0x55);
}
// Add more STA addressing mode tests as you implement them
