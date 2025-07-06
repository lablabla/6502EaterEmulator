#pragma once

#include "devices/Device.h"

#include <array>

namespace EaterEmulator 
{
    // W65C22 is a versatile I/O chip
    class VIAW65C22 : public Device
    {
    public:
        VIAW65C22();
        ~VIAW65C22() = default; // Default destructor

        VIAW65C22(const VIAW65C22&) = default;
        VIAW65C22& operator=(const VIAW65C22&) = default;

        VIAW65C22(VIAW65C22&&) = default;
        VIAW65C22& operator=(VIAW65C22&&) = default;
        
        bool shouldHandlePins(CPU::Pins& pins) override; // Override the check method from Device
        void handlePins(CPU::Pins& pins) override; // Override the action method from Device
        std::string getName() const override { return "VIAW65C22"; } // Override the name method from Device

        struct Registers
        {
            uint8_t porta; // Port A register
            uint8_t portb; // Port B register
            uint8_t ddra; // Data direction register for port A
            uint8_t ddrb; // Data direction register for port B
            uint8_t t1cl; // Timer 1 low byte
            uint8_t t1ch; // Timer 1 high byte
            uint8_t t1ll; // Timer 1 latch low byte
            uint8_t t1lh; // Timer 1 latch high byte
            uint8_t t2cl; // Timer 2 low byte
            uint8_t t2ch; // Timer 2 high byte
            uint8_t sr;   // Shift register
            uint8_t acr;  // Auxiliary control register
            uint8_t pcr;  // Peripheral control register
            uint8_t ifr;  // Interrupt flag register
            uint8_t ier;  // Interrupt enable register
        };

    private:
        uint8_t& getRegister(uint8_t address);

        Registers registers; // Registers for the VIAW65C22
    };
} // namespace EaterEmulator