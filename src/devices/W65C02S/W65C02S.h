#pragma once

#include "core/bus_master.h"
#include "core/defines.h"
#include "devices/W65C02S/opcodes.h"

namespace EaterEmulator::devices
{
    // Status flags bits
    static constexpr uint8_t STATUS_CARRY = 0x01; // Carry Flag
    static constexpr uint8_t STATUS_ZERO = 0x02; // Zero Flag
    static constexpr uint8_t STATUS_INTERRUPT = 0x04; // Interrupt Disable Flag
    static constexpr uint8_t STATUS_DECIMAL = 0x08; // Decimal Mode Flag
    static constexpr uint8_t STATUS_BREAK = 0x10; // Break Command Flag
    static constexpr uint8_t STATUS_UNUSED = 0x20; // Unused Flag (always 1)
    static constexpr uint8_t STATUS_OVERFLOW = 0x40; // Overflow Flag
    static constexpr uint8_t STATUS_NEGATIVE = 0x80; // Negative Flag

    static constexpr uint16_t RESET_VECTOR = 0xFFFC; // Reset vector address
    static constexpr uint16_t IRQ_BRK_VECTOR = 0xFFFE; // IRQ vector address
    static constexpr uint16_t NMI_VECTOR = 0xFFFA; // NMI vector address


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

        void handleClockStateChange(core::State state) override;
        void notifyBus(uint8_t rwb) override;
        void setIRQ(core::State state);
        void setNMI(core::State state);

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
        void handleReset();

        // Accumulator addressing modes
        [[nodiscard]]bool handleAccumulatorAddressing(const OpcodeInfo& info, core::State clockState);
        [[nodiscard]]bool handleAccumulatorLow(const OpcodeInfo& info);
        [[nodiscard]]bool handleAccumulatorHigh(const OpcodeInfo& info);

        // Implied addressing modes
        [[nodiscard]]bool handleImpliedAddressing(const OpcodeInfo& info, core::State clockState);
        [[nodiscard]]bool handleImpliedLow(const OpcodeInfo& info);
        [[nodiscard]]bool handleImpliedHigh(const OpcodeInfo& info);

        // Immediate
        [[nodiscard]]bool handleImmediateAddressing(const OpcodeInfo& info,  core::State clockState);
        [[nodiscard]]bool handleImmediateLow(const OpcodeInfo& info);
        [[nodiscard]]bool handleImmediateHigh(const OpcodeInfo& info);

        // Absolute
        [[nodiscard]]bool handleAbsoluteAddressing(const OpcodeInfo& info, core::State clockState);
        [[nodiscard]]bool handleAbsoluteLow(const OpcodeInfo& info);
        [[nodiscard]]bool handleAbsoluteHigh(const OpcodeInfo& info);

        [[nodiscard]]bool handleAbsoluteIndexedAddressing(const OpcodeInfo& info, core::State clockState);
        [[nodiscard]]bool handleAbsoluteIndexedLow(const OpcodeInfo& info);
        [[nodiscard]]bool handleAbsoluteIndexedHigh(const OpcodeInfo& info);

        // Relative
        [[nodiscard]]bool handleRelativeAddressing(const OpcodeInfo& info, core::State clockState);
        [[nodiscard]]bool handleRelativeLow(const OpcodeInfo& info);
        [[nodiscard]]bool handleRelativeHigh(const OpcodeInfo& info);

        // Zero Page
        [[nodiscard]]bool handleZeroPageAddressing(const OpcodeInfo& info, core::State clockState);
        [[nodiscard]]bool handleZeroPageLow(const OpcodeInfo& info);
        [[nodiscard]]bool handleZeroPageHigh(const OpcodeInfo& info);

        [[nodiscard]]bool handleZeroPageIndexedAddressing(const OpcodeInfo& info, core::State clockState);
        [[nodiscard]]bool handleZeroPageIndexedLow(const OpcodeInfo& info);
        [[nodiscard]]bool handleZeroPageIndexedHigh(const OpcodeInfo& info);

        // Math
        void doAND();
        void doORA();
        void doEOR();
        void doADC();
        void doSBC();

        void doBIT();
        void doASL();
        void doLSR();
        void doROL();
        void doROR();

        uint8_t fetchByte();
        void writeByte(uint8_t data);

        void updateStatusFlags(uint8_t value);

        // Registers
        uint8_t _a; // Accumulator
        uint8_t _x; // X Register
        uint8_t _y; // Y Register
        uint8_t _sp; // Stack Pointer
        uint16_t _pc; // Program Counter
        uint8_t _status; // Processor Status

        uint16_t _interruptVector = RESET_VECTOR;
        bool _interruptFromSW = true;

        Opcode _ir; // Instruction Register
        uint8_t _adl; // Address Low Byte
        uint8_t _adh; // Address High Byte

        core::State _irq = core::HIGH;
        core::State _nmi = core::HIGH;
        int _cycle = 0;

        bool _started = false;
        uint8_t _resetStage = 0;
    };
};