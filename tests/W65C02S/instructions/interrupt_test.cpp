// Test suite for interrupt handling (BRK, IRQ, NMI)
#include "cpu_instruction_test.h"
#include "devices/SRAM62256/SRAM62256.h"
#include <cstdint>

using namespace EaterEmulator;

class InterruptTest : public CPUInstructionTest {
protected:
    std::unique_ptr<devices::SRAM62256> ram;
    
    void SetUp() override {
        CPUInstructionTest::SetUp();
        ram = std::make_unique<devices::SRAM62256>(bus);
        bus.addSlave(ram.get());
    }
    
    void TearDown() override {
        ram.reset();
        CPUInstructionTest::TearDown();
    }
    
    void setupInterruptVectors() {
        // Set up interrupt vectors in memory
        // IRQ/BRK vector at 0xFFFE/0xFFFF
        memory[0xFFFE - MEMORY_OFFSET] = 0x00; // IRQ vector low
        memory[0xFFFF - MEMORY_OFFSET] = 0x90; // IRQ vector high (0x9000)
        
        // NMI vector at 0xFFFA/0xFFFB  
        memory[0xFFFA - MEMORY_OFFSET] = 0x00; // NMI vector low
        memory[0xFFFB - MEMORY_OFFSET] = 0x92; // NMI vector high (0x9200)
        
        // Reset vector at 0xFFFC/0xFFFD
        memory[0xFFFC - MEMORY_OFFSET] = 0x00; // Reset vector low
        memory[0xFFFD - MEMORY_OFFSET] = 0x80; // Reset vector high (0x8000)
    }
};

TEST_F(InterruptTest, BRK_ExecutesCorrectly) {
    setupInterruptVectors();
    
    auto opcode = Opcode::BRK;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    // Set up BRK instruction at reset vector
    memory[0x8000 - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    memory[0x8001 - MEMORY_OFFSET] = 0x00; // BRK operand (ignored)
    
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    // Set initial state
    uint16_t initialPC = cpu->getProgramCounter();
    uint8_t initialStatus = cpu->getStatus();
    uint8_t initialSP = cpu->getStackPointer();
    
    // Execute BRK instruction
    for (int i = 0; i < cycles; ++i) {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    // Verify BRK behavior
    EXPECT_EQ(cpu->getProgramCounter(), 0x9000); // Should jump to IRQ vector
    EXPECT_EQ(cpu->getStackPointer(), initialSP - 3); // Should push 3 bytes to stack
    
    // Verify stack contents (return address and status)
    auto& stackMemory = ram->getMemory();
    EXPECT_EQ(stackMemory[0x0100 + initialSP], static_cast<uint8_t>((initialPC + 2) >> 8)); // Return address high
    EXPECT_EQ(stackMemory[0x0100 + initialSP - 1], static_cast<uint8_t>(initialPC + 2)); // Return address low  
    EXPECT_EQ(stackMemory[0x0100 + initialSP - 2], initialStatus | devices::STATUS_BREAK); // Status with B flag set
    
    // Verify interrupt disable flag is set
    EXPECT_EQ(cpu->getStatus() & devices::STATUS_INTERRUPT, devices::STATUS_INTERRUPT);
}

TEST_F(InterruptTest, BRK_SetsBreakFlag) {
    setupInterruptVectors();
    
    auto opcode = Opcode::BRK;
    auto it = OpcodeMap.find(opcode);
    ASSERT_NE(it, OpcodeMap.end()) << "Opcode not found in map";
    auto cycles = it->second.cycles;
    
    memory[0x8000 - MEMORY_OFFSET] = static_cast<uint8_t>(opcode);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    uint8_t initialSP = cpu->getStackPointer();
    
    for (int i = 0; i < cycles; ++i) {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    // Check that break flag was pushed to stack
    auto& stackMemory = ram->getMemory();
    uint8_t pushedStatus = stackMemory[0x0100 + initialSP - 2];
    EXPECT_EQ(pushedStatus & devices::STATUS_BREAK, devices::STATUS_BREAK);
}

TEST_F(InterruptTest, IRQ_TriggersWhenEnabled) {
    setupInterruptVectors();
    
    // Set up NOP instruction at reset vector
    memory[0x8000 - MEMORY_OFFSET] = static_cast<uint8_t>(Opcode::NOP);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    // Ensure interrupt flag is clear (interrupts enabled)
    cpu->setStatus(cpu->getStatus() & ~devices::STATUS_INTERRUPT);
    
    uint16_t initialPC = cpu->getProgramCounter();
    uint8_t initialSP = cpu->getStackPointer();
    
    // Trigger IRQ
    cpu->setIRQ(core::LOW);
    
    // Execute one instruction cycle - should detect IRQ during instruction fetch
    cpu->handleClockStateChange(core::LOW);
    cpu->handleClockStateChange(core::HIGH);
    
    // Continue for BRK cycles (7 cycles total for IRQ handling)
    for (int i = 1; i < 7; ++i) {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    // Verify IRQ was handled
    EXPECT_EQ(cpu->getProgramCounter(), 0x9000); // Should jump to IRQ vector
    EXPECT_EQ(cpu->getStackPointer(), initialSP - 3); // Should push 3 bytes to stack
    
    // Verify stack contents
    auto& stackMemory = ram->getMemory();
    EXPECT_EQ(stackMemory[0x0100 + initialSP], static_cast<uint8_t>(initialPC >> 8)); // Return address high
    EXPECT_EQ(stackMemory[0x0100 + initialSP - 1], static_cast<uint8_t>(initialPC)); // Return address low
    
    // Verify interrupt disable flag is set
    EXPECT_EQ(cpu->getStatus() & devices::STATUS_INTERRUPT, devices::STATUS_INTERRUPT);
}

TEST_F(InterruptTest, IRQ_IgnoredWhenDisabled) {
    setupInterruptVectors();
    
    // Set up NOP instruction at reset vector
    memory[0x8000 - MEMORY_OFFSET] = static_cast<uint8_t>(Opcode::NOP);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    // Disable interrupts
    cpu->setStatus(cpu->getStatus() | devices::STATUS_INTERRUPT);
    
    uint16_t initialPC = cpu->getProgramCounter();
    uint8_t initialSP = cpu->getStackPointer();
    
    // Trigger IRQ
    cpu->setIRQ(core::LOW);
    
    // Execute NOP instruction (2 cycles)
    for (int i = 0; i < 2; ++i) {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    // Verify IRQ was ignored
    EXPECT_EQ(cpu->getProgramCounter(), initialPC + 1); // Should have executed NOP normally
    EXPECT_EQ(cpu->getStackPointer(), initialSP); // Stack should be unchanged
}

TEST_F(InterruptTest, NMI_TriggersRegardlessOfInterruptFlag) {
    setupInterruptVectors();
    
    // Set up NOP instruction at reset vector
    memory[0x8000 - MEMORY_OFFSET] = static_cast<uint8_t>(Opcode::NOP);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    // Disable interrupts (NMI should still work)
    cpu->setStatus(cpu->getStatus() | devices::STATUS_INTERRUPT);
    
    uint16_t initialPC = cpu->getProgramCounter();
    uint8_t initialSP = cpu->getStackPointer();
    
    // Trigger NMI
    cpu->setNMI(core::LOW);
    
    // Execute one instruction cycle - should detect NMI during instruction fetch
    cpu->handleClockStateChange(core::LOW);
    cpu->handleClockStateChange(core::HIGH);
    
    // Continue for BRK cycles (7 cycles total for NMI handling)
    for (int i = 1; i < 7; ++i) {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    // Verify NMI was handled
    EXPECT_EQ(cpu->getProgramCounter(), 0x9200); // Should jump to NMI vector
    EXPECT_EQ(cpu->getStackPointer(), initialSP - 3); // Should push 3 bytes to stack
    
    // Verify stack contents
    auto& stackMemory = ram->getMemory();
    EXPECT_EQ(stackMemory[0x0100 + initialSP], static_cast<uint8_t>(initialPC >> 8)); // Return address high
    EXPECT_EQ(stackMemory[0x0100 + initialSP - 1], static_cast<uint8_t>(initialPC)); // Return address low
    
    // Verify interrupt disable flag is set
    EXPECT_EQ(cpu->getStatus() & devices::STATUS_INTERRUPT, devices::STATUS_INTERRUPT);
}

TEST_F(InterruptTest, RTI_RestoresStateCorrectly) {
    setupInterruptVectors();
    
    // Set up RTI instruction at IRQ vector
    memory[0x9000 - MEMORY_OFFSET] = static_cast<uint8_t>(Opcode::RTI);
    
    // Set up initial instruction at reset vector
    memory[0x8000 - MEMORY_OFFSET] = static_cast<uint8_t>(Opcode::NOP);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    // Manually set up stack as if we came from an interrupt
    uint16_t returnAddress = 0x8001;
    uint8_t savedStatus = 0x24; // Some test status value
    uint8_t sp = cpu->getStackPointer();
    
    auto& stackMemory = ram->getMemory();
    stackMemory[0x0100 + sp] = static_cast<uint8_t>(returnAddress >> 8); // Return address high
    stackMemory[0x0100 + sp - 1] = static_cast<uint8_t>(returnAddress); // Return address low
    stackMemory[0x0100 + sp - 2] = savedStatus; // Saved status
    cpu->setStackPointer(sp - 3); // Adjust SP as if interrupt occurred
    
    // Set PC to RTI instruction
    cpu->setProgramCounter(0x9000);
    
    // Execute RTI instruction (6 cycles)
    auto it = OpcodeMap.find(Opcode::RTI);
    auto cycles = it->second.cycles;
    for (int i = 0; i < cycles; ++i) {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    // Verify RTI behavior
    EXPECT_EQ(cpu->getProgramCounter(), returnAddress); // Should return to saved address
    EXPECT_EQ(cpu->getStackPointer(), sp); // Should restore stack pointer
    EXPECT_EQ(cpu->getStatus() & ~devices::STATUS_BREAK, savedStatus & ~devices::STATUS_BREAK); // Should restore status (except B flag)
}

TEST_F(InterruptTest, InterruptPriority_NMI_OverIRQ) {
    setupInterruptVectors();
    
    // Set up NOP instruction at reset vector
    memory[0x8000 - MEMORY_OFFSET] = static_cast<uint8_t>(Opcode::NOP);
    rom = std::make_unique<devices::EEPROM28C256>(memory, bus);
    bus.addSlave(rom.get());
    
    // Enable interrupts
    cpu->setStatus(cpu->getStatus() & ~devices::STATUS_INTERRUPT);
    
    // Trigger both NMI and IRQ simultaneously
    cpu->setNMI(core::LOW);
    cpu->setIRQ(core::LOW);
    
    // Execute one instruction cycle
    cpu->handleClockStateChange(core::LOW);
    cpu->handleClockStateChange(core::HIGH);
    
    // Continue for interrupt handling cycles
    for (int i = 1; i < 7; ++i) {
        cpu->handleClockStateChange(core::LOW);
        cpu->handleClockStateChange(core::HIGH);
    }
    
    // Verify NMI took priority (should jump to NMI vector, not IRQ vector)
    EXPECT_EQ(cpu->getProgramCounter(), 0x9200); // NMI vector, not IRQ vector
}
