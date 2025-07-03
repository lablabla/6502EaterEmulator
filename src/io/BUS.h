#pragma once

#include <array>
#include <cstdint>

namespace EaterEmulator 
{
    enum class PinValue 
    {
        LOW = 0,
        HIGH = 1
    };

    template<int T>
    class BUS 
    {
    public:
        BUS()
        {
            _pins.fill(PinValue::LOW); // Initialize all pins to LOW
        }
        ~BUS() = default; // Default destructor

        BUS(const BUS&) = default;
        BUS& operator=(const BUS&) = default;
        BUS(BUS&&) = default;
        BUS& operator=(BUS&&) = default;

        // Set the value of a pin
        void setPin(int index, PinValue value)
        {
            if (index >= 0 && index < T)
            {
                _pins[index] = value; // Set the pin value
            }
        }
        // Get the value of a pin
        PinValue getPin(int index) const
        {
            if (index >= 0 && index < T)
            {
                return _pins[index]; // Return the pin value
            }
            return PinValue::LOW; // Return LOW if index is out of bounds
        }

        uint16_t getAddress() const
        {
            uint16_t address = 0;
            for (int i = 0; i < T; ++i)
            {
                if (_pins[i] == PinValue::HIGH)
                {
                    address |= (1 << i); // Set the corresponding bit in the address
                }
            }
            return address; // Return the constructed address
        }

        void setAddress(uint16_t address)
        {
            for (int i = 0; i < T; ++i)
            {
                if (address & (1 << i))
                {
                    _pins[i] = PinValue::HIGH; // Set the pin to HIGH if the corresponding bit is set
                }
                else
                {
                    _pins[i] = PinValue::LOW; // Set the pin to LOW otherwise
                }
            }
        }

        uint8_t getData() const
        {
            uint8_t data = 0;
            for (int i = 0; i < T; ++i)
            {
                if (_pins[i] == PinValue::HIGH)
                {
                    data |= (1 << i); // Set the corresponding bit in the data
                }
            }
            return data; // Return the constructed data
        }

        void setData(uint8_t data)
        {
            for (int i = 0; i < T; ++i)
            {
                if (data & (1 << i))
                {
                    _pins[i] = PinValue::HIGH; // Set the pin to HIGH if the corresponding bit is set
                }
                else
                {
                    _pins[i] = PinValue::LOW; // Set the pin to LOW otherwise
                }
            }
        }

    private:
        std::array<PinValue, T> _pins; // Array to hold pin values
    };

    using AddressBus = BUS<16>; // 16-bit address bus
    using DataBus = BUS<8>; // 8-bit data bus
} // namespace EaterEmulator