// Test suite for ADC opcode
#include "cpu_instruction_test.h"

using namespace EaterEmulator;

TEST_F(CPUInstructionTest, ADC_IMM_AddsImmediateValue) {
    cpu.a() = 0x10;
    memory[0x0000] = static_cast<uint8_t>(Opcode::ADC_IMM);
    memory[0x0001] = 0x22;
    cpu.pc() = 0x0000;
    cpu.runNextInstruction();
    EXPECT_EQ(cpu.a(), 0x32);
}
// Add more ADC addressing mode tests as you implement them
