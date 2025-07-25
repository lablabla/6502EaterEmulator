
#include "devices/HD44780LCD/HD44780LCD.h"

#include "spdlog/spdlog.h"

#include <sys/types.h>
#include <iostream>
#include <bit>

namespace EaterEmulator::devices
{
    HD44780LCD::HD44780LCD() 
    {
    }

    HD44780LCD::~HD44780LCD() 
    {
        spdlog::debug("HD44780LCD destroyed.");
    }

    void HD44780LCD::setControlLines(uint8_t rs, uint8_t rw, bool enable)
    {
        _rs = rs;
        _rw = rw;
        checkEnableToggled(enable);
    }

    void HD44780LCD::writeDataLines(uint8_t data)
    {
        _data = data;
    }

    void HD44780LCD::checkEnableToggled(bool newEnabled)
    {
        // Data/contol are updated on the falling edge of the enable signal. If it was high and now low, we should process the current state of the LCD
        if (_enable && !newEnabled)
        {
            if (_rw == 0)
            {
                handleWrite();
            }
            else {
                handleRead();
            }
        }
        _enable = newEnabled;
    }

    void HD44780LCD::handleWrite()
    {
        if (_rs == 0)
        {
            _instructionRegister = _data;

            // Handle instruction write
            // Get upper most bit to determine the instruction
            auto instruction = std::bit_width(_instructionRegister) - 1;
            switch(instruction)
            {
                case 0: // Clear display
                    _ddram.fill(0);
                    _addressCounter = 0;
                    break;
                case 1: // Return home
                    _addressCounter = 0;
                    break;
                case 2: // Entry mode set

                    break;
                case 3: // Display on/off control
                    _displayOn = (_instructionRegister & 0b00000100) != 0;
                    _cursorOn = (_instructionRegister & 0b00000010) != 0;
                    _blinkOn = (_instructionRegister & 0b00000001) != 0;
                    break;
                case 4: // Cursor display shift
                    break;
                case 5: // Function set
                    _bitsMode =  (_instructionRegister & 0b00010000) ? BitsMode::MODE_8 : BitsMode::MODE_4;
                    _linesMode = (_instructionRegister & 0b00001000) ? LinesMode::LINES_2 : LinesMode::LINES_1;
                    break;
                case 6: // Set CGRAM address
                    // break;
                case 7: // Set DDRAM address
                    // break;
                default:
                    spdlog::error("Unhandled instruction: {:#04x}", static_cast<int>(_instructionRegister));
                    break;
            }
        }
        else
        {
            _dataRegister = _data;
        }
        update();
    }

    void HD44780LCD::handleRead()
    {
        
    }

    void HD44780LCD::update()
    {
        if (_rs == 0)
        {
        }
        else
        {
            // Update data register
            _ddram[_addressCounter] = _dataRegister;
            _addressCounter = (_addressCounter + 1) % _ddram.size(); // Increment address counter, wrap around if necessary

        }
        std::cout << "\r";
        for (const auto& character : _ddram)
        {
            std::cout << static_cast<char>(character);
        }
        std::flush(std::cout);
    }

} // namespace EaterEmulator