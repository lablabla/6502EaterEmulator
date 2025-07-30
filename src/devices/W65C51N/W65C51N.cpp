
#include "devices/W65C51N/W65C51N.h"

#include "core/bus.h"
#include "core/defines.h"
#include "spdlog/spdlog.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sys/types.h>
 #include <termios.h> // For termios functions
#include <unistd.h>  // For STDIN_FILENO

namespace EaterEmulator::devices
{
    W65C51N::W65C51N(std::shared_ptr<core::Bus> bus) 
        : core::BusSlave(bus, 0x5000)
    {
        // Initialize the background thread for handling communication
        _backgroundThread = std::jthread([this](std::stop_token st) 
        {
            struct termios oldt, newt;

            // Get current terminal settings
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;

            // Disable canonical mode (line buffering) and echoing
            newt.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            while (!st.stop_requested()) 
            {
                auto c = std::getchar();
                if (c == EOF) {
                    spdlog::error("Error reading from standard input.");
                    continue;
                }
                this->_receiveData = static_cast<uint8_t>(c);
                // If Receiver Data Register is already full, set the overrun flag
                if (this->_status & (1 << 3))
                {
                    this->_status |= (1 << 2);
                }
                this->_status |= (1 << 3); // Set Receiver Data Register Full bit to full
            }
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        });
    }

    W65C51N::~W65C51N() 
    {
        spdlog::debug("W65C51N destroyed.");
    }

    void W65C51N::handleBusNotification(uint16_t address, uint8_t rwb)
    {
        if (!shouldHandleAddress(address)) {
            spdlog::debug("W65C51N: Address {:#04x} not handled by this device", address);
            return; // If the pins are not for this device, do nothing
        }

        auto reg = static_cast<Register>((address - _offset) & 0x0F);
        if (rwb == core::READ)
        {
            handleRead(reg);
        }
        else 
        {
            handleWrite(reg);            
        }
    }

    bool W65C51N::shouldHandleAddress(const uint16_t& address) const
    {
        // Decode address from the address bus and check if it falls within the range of this VIA
        // ACIA is mapped to 0x5000 - 0x50FF so only when A12 and A14 are HIGH and A15 is LOW.
        // To avoid collision with VIA, A13 must be LOW.
        return (address & (1 << 15)) == 0 && (address & (1 << 14)) != 0 && (address & (1 << 13)) == 0 && (address & (1 << 12)) != 0; 
    }

    bool W65C51N::handleRead(Register reg)
    {
        uint8_t data = readRegister(reg);
        _bus->setData(data);
        return true;
    }

    bool W65C51N::handleWrite(Register reg)
    {
        uint8_t data;
        _bus->getData(data);

        switch(reg)
        {            
            case Register::DATA:
                _transmitData = data;
                std::cout << static_cast<char>(_transmitData);
                std::flush(std::cout);
                break;
            case Register::STATUS:
                _status = data;
                break;
            case Register::COMMAND:
                _command = data;
                break;
            case Register::CONTROL:
                _control = data;
                break;
            default:
                spdlog::error("W65C51N: Invalid register for write operation: {:#04x}", static_cast<int>(reg));
                return false;
        }

        return true;
    }

    uint8_t W65C51N::readRegister(Register reg)
    {
        switch (reg)
        {
            case Register::DATA: 
                {
                    auto mask = ~(1 << 3) & ~(1 << 2);
                    _status = _status & mask;
                    return _receiveData;
                }
            case Register::STATUS: return _status;
            case Register::COMMAND: return _command;
            case Register::CONTROL: return _control;
            default:
                spdlog::error("W65C51N: Invalid register: {:#04x}", static_cast<int>(reg));
                return 0;
        }
    }
} // namespace EaterEmulator