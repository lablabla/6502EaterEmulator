#pragma once

#include "core/bus_master.h"
#include "core/defines.h"
#include "devices/W65C02S/opcodes.h"

namespace EaterEmulator::devices
{
    // Status flags bits
    static constexpr uint8_t STATUS_CARRY = 0x01; // Carry Flag
    static constexpr uint8_t STATUS_ZERO = 0x02; // Zero Flag
    static constexpr uint8_t STATUS_OVERFLOW = 0x40; // Overflow Flag
    static constexpr uint8_t STATUS_NEGATIVE = 0x80; // Negative Flag


    // W65C02S CPU device class
    class W65C02S : public core::BusMaster
    {
    public:
        W65C02S(core::Bus& bus);
        virtual ~W65C02S();

        // Non-copyable, Non-movable
        W65C02S(const W65C02S&) = delete;
        W65C02S& operator=(const W65C02S&) = delete;
        W65C02S(W65C02S&&) = delete;
        W65C02S& operator=(W65C02S&&) = delete;

        // Reset the CPU
        void reset();

        void handleClockStateChange(core::ClockState state) override;
        void notifyBus(uint8_t rwb) override;

        std::string getName() const override { return "W65C02S"; }

#ifdef UNIT_TEST
        // For unit testing purposes

        void setAccumulator(uint8_t value) { _a = value; }
        void setXRegister(uint8_t value) { _x = value; }
        void setYRegister(uint8_t value) { _y = value; }
        void setStackPointer(uint8_t value) { _sp = value; }
        void setProgramCounter(uint16_t value) { _pc = value; }
        void setStatus(uint8_t value) { _status = value; }
        void setResetStage(uint8_t stage) { _resetStage = stage; }

        uint8_t getAccumulator() const { return _a; }
        uint8_t getXRegister() const { return _x; }
        uint8_t getYRegister() const { return _y; }
        uint8_t getStackPointer() const { return _sp; }
        uint16_t getProgramCounter() const { return _pc; }
        uint8_t getStatus() const { return _status; }
        Opcode getInstructionRegister() const { return _ir; }
        uint8_t getAddressLow() const { return _adl; }
        uint8_t getAddressHigh() const { return _adh; }
#endif

    private:

        void handlePhi2Low();
        void handlePhi2High();

        void handlePhi2LowAddressing();
        void handlePhi2HighAddressing();
        void handleReset();

        [[nodiscard]]bool handleImmediateAddressing(const OpcodeInfo& info,  core::ClockState clockState);
        [[nodiscard]]bool handleImmediateLow(const OpcodeInfo& info);
        [[nodiscard]]bool handleImmediateHigh(const OpcodeInfo& info);

        [[nodiscard]]bool handleAbsoluteAddressing(const OpcodeInfo& info, core::ClockState clockState);
        [[nodiscard]]bool handleAbsoluteLow(const OpcodeInfo& info);
        [[nodiscard]]bool handleAbsoluteHigh(const OpcodeInfo& info);

        [[nodiscard]]bool handleZeroPageAddressing(const OpcodeInfo& info, core::ClockState clockState);
        [[nodiscard]]bool handleZeroPageLow(const OpcodeInfo& info);
        [[nodiscard]]bool handleZeroPageHigh(const OpcodeInfo& info);
        
        // Handle stages
        void handleOpcode(uint8_t rwb);
        void handleStage1(const OpcodeInfo& info);
        void handleStage2(const OpcodeInfo& info);
        void handleStage3(const OpcodeInfo& info);
        void handleStage4(const OpcodeInfo& info);

        uint8_t fetchByte();
        void writeByte(uint8_t data);

        void executeInstruction(Opcode opcode);
        void updateStatusFlags(uint8_t value);
        uint8_t getRWB() const;
        bool shouldIncrementPC(AddressingMode mode) const;

        // Registers
        uint8_t _a; // Accumulator
        uint8_t _x; // X Register
        uint8_t _y; // Y Register
        uint8_t _sp; // Stack Pointer
        uint16_t _pc; // Program Counter
        uint8_t _status; // Processor Status

        Opcode _ir; // Instruction Register
        uint8_t _adl; // Address Low Byte
        uint8_t _adh; // Address High Byte

        int _stage = 0; // Current stage of instruction execution
        int _cycle = 0;

        bool _started = false;
        uint8_t _resetStage = 0;
    };
};