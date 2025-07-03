#pragma once

#include "cpu/CPU.h" 

#include <string>

namespace EaterEmulator 
{
    class IODevice 
    {
    public:
        IODevice(uint16_t offset = 0): _offset(offset){} // Default constructor
        virtual ~IODevice() = default; // Default destructor

        IODevice(const IODevice&) = default;
        IODevice& operator=(const IODevice&) = default;
        IODevice(IODevice&&) = default; 
        IODevice& operator=(IODevice&&) = default;
        
        virtual bool shouldHandlePins(CPU::Pins& pins) = 0; // Pure virtual function to check if device can handle pins
        virtual void handlePins(CPU::Pins& pins) = 0; // Pure virtual function for device action
        virtual std::string getName() const = 0;

    protected:
        uint16_t _offset = 0;
    };
} // namespace EaterEmulator