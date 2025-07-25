#pragma once

#include <cstdint>
#include <array>

namespace EaterEmulator::devices
{
    // HD44780LCD is a common LCD controller
    class HD44780LCD
    {
    public:
        HD44780LCD();
        virtual ~HD44780LCD();

        HD44780LCD(const HD44780LCD&) = delete;
        HD44780LCD& operator=(const HD44780LCD&) = delete;

        HD44780LCD(HD44780LCD&&) = delete;
        HD44780LCD& operator=(HD44780LCD&&) = delete;

        void setControlLines(uint8_t rs, uint8_t rw, bool enable);
        void writeDataLines(uint8_t data);
    private:

        enum BitsMode
        {
            MODE_4 = 0,
            MODE_8 = 1
        };

        enum LinesMode
        {
            LINES_1 = 0,
            LINES_2 = 1
        };



        void checkEnableToggled(bool newEnabled);

        void handleWrite();
        void handleRead();

        void update();

        uint8_t _rs;
        uint8_t _rw;
        bool _enable;
        uint8_t _data;
        uint8_t _dataRegister;
        uint8_t _instructionRegister;
        uint8_t _addressCounter;


        bool _displayOn;
        bool _cursorOn;
        bool _blinkOn;
        BitsMode _bitsMode;
        LinesMode _linesMode;
        
        std::array<uint8_t, 80> _ddram;
    };
} // namespace EaterEmulator