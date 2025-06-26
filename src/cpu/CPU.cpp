#include "cpu/CPU.h"

namespace ya6502e 
{
    CPU::CPU(const uint8_t* const rom, size_t romSize)
    {
        // Constructor implementation
        reset(); // Initialize CPU state
        this->rom = rom;
        this->romSize = romSize;
    }

    void CPU::tick() 
    {
        // Execute one CPU cycle
        // This is where the main logic for executing instructions would go
    }

    void CPU::reset() 
    {
        // Reset the CPU state
        x = 0;
        y = 0;
        a = 0;
        status = 0;
        pc = 0xFFFC; // Typical reset vector for 6502
        sp = 0xFD; // Stack pointer starts at 0xFD
    }

    uint8_t CPU::fetch() 
    {
        return rom[pc++]; // Fetch the next byte from ROM
    }
} // namespace ya6502e