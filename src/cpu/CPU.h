#pragma once

#include "cpu/Opcodes.h"
#include "devices/BUS.h"

#include <cstdint>
#include <cstddef> // for size_t
#include <sys/types.h>
#include <vector>
#include <memory>

namespace EaterEmulator 
{
    enum class StatusBits
    {
        CarryFlag = 0x01, // Carry flag bit in status register
        ZeroFlag = 0x02, // Zero flag bit in status register
        InterruptDisableFlag = 0x04, // Interrupt disable flag bit in status register
        DecimalFlag = 0x08, // Decimal mode flag bit in status register
        OverflowFlag = 0x40, // Overflow flag bit in status register
        NegativeFlag = 0x80 // Negative flag bit in status register
    };

    class Device; // Forward declaration of Device

    class CPU 
    {
    public:
        CPU(DataBus& dataBus, AddressBus& addressBus);
        ~CPU() = default; // Default destructor

        CPU(const CPU&) = delete; // Disable copy constructor
        CPU& operator=(const CPU&) = delete; // Disable copy assignment

        CPU(CPU&&) = delete; // Disable move constructor
        CPU& operator=(CPU&&) = delete; // Disable move assignment

        void runNextInstruction(); // Execute the next instruction
        
        void reset(); // Reset the CPU state
        
        void setDevices(const std::vector<std::shared_ptr<Device>>& devices); // Set the devices for the CPU

        struct Registers
        {
            uint8_t x; // X register
            uint8_t y; // Y register
            uint8_t a; // Accumulator
            uint8_t status; // Status register
            uint16_t pc; // Program counter
            uint8_t sp; // Stack pointer
        };

        struct Pins
        {
            AddressBus addressBus; // Address bus
            DataBus dataBus; // Data bus
            PinValue RWB; // Read/Write-Bar control signal
        };

#if defined(UNIT_TEST)
        uint8_t& x() { return registers.x; } // Accessor for the X register in unit tests
        uint8_t& y() { return registers.y; } // Accessor for the Y register in unit tests
        uint8_t& a() { return registers.a; } // Accessor for the accumulator in unit tests
        uint8_t& status() { return registers.status; } // Accessor for the status register in unit tests
        uint16_t& pc() { return registers.pc; } // Accessor for the program counter in unit tests
        uint8_t& sp() { return registers.sp; } // Accessor for the stack pointer in unit tests
#endif
    private:
        // Device interaction methods
        void setAddressBus(uint16_t address);
        void setDataBus(uint8_t data);
        void getDataBus(uint8_t& data);
        void notifyDevices();

        // Opcode handling methods
        void handleOpcode(Opcode opcode); // Handle the opcode execution
        void decodeAddressingMode(AddressingMode addressingMode, std::vector<uint8_t>& operands); // Decode the addressing mode for the opcode

        void updateStatusFlag(StatusBits flag, bool value);
        void setStatusFlag(StatusBits flag);
        void clearStatusFlag(StatusBits flag);


        // Private methods for instruction execution, memory access, etc.
        uint16_t fetchAddress(uint16_t offset); // Fetch the next opcode from memory
        uint8_t fetchByte(uint16_t address);
        void writeByte(uint16_t address, uint8_t value);

        Registers registers; // CPU registers

        Pins _pins; // Pins for the CPU
        std::vector<std::shared_ptr<Device>> _devices; // List of I/O devices

        static constexpr size_t memorySize = 0x10000; // 64KB address space for 6502
    };
} // namespace EaterEmulator