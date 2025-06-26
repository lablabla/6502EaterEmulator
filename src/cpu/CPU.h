#pragma once

#include <cstdint>
#include <cstddef> // for size_t
#include <array>

namespace ya6502e 
{
    class CPU 
    {
    public:
        CPU(const uint8_t* const rom, size_t romSize);
        ~CPU() = default; // Default destructor

        CPU(const CPU&) = delete; // Disable copy constructor
        CPU& operator=(const CPU&) = delete; // Disable copy assignment

        CPU(CPU&&) = delete; // Disable move constructor
        CPU& operator=(CPU&&) = delete; // Disable move assignment

        void tick(); // Execute one CPU cycle
        void reset(); // Reset the CPU state

    private:
        // Private methods for instruction execution, memory access, etc.
        uint8_t fetch(); // Fetch the next byte from memory

        // Private members for CPU state, registers, etc.
        uint8_t x;
        uint8_t y;
        uint8_t a; // Accumulator
        uint8_t status; // Status register
        uint16_t pc; // Program counter
        uint8_t sp; // Stack pointer

        std::array<uint8_t, 0xFFFF> memory; // Memory array (64KB)

        const uint8_t* rom; // Pointer to the ROM
        size_t romSize; // Size of the ROM
    };
} // namespace ya6502e