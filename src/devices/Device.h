#pragma once

#include "cpu/CPU.h" 

#include <string>

namespace EaterEmulator 
{
    class Device 
    {
    public:
        Device(uint16_t offset = 0): _offset(offset){} // Default constructor
        virtual ~Device() = default; // Default destructor

        Device(const Device&) = default;
        Device& operator=(const Device&) = default;
        Device(Device&&) = default; 
        Device& operator=(Device&&) = default;
        
        virtual bool shouldHandlePins(CPU::Pins& pins) = 0; // Pure virtual function to check if device can handle pins
        virtual void handlePins(CPU::Pins& pins) = 0; // Pure virtual function for device action
        virtual std::string getName() const = 0;

    protected:
        uint16_t _offset = 0;
    };
} // namespace EaterEmulator