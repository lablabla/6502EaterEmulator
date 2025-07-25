
#include "devices/HD44780LCD/LCDAdapter.h"
#include "devices/HD44780LCD/HD44780LCD.h"

#include "spdlog/spdlog.h"

namespace EaterEmulator::devices
{
    LCDAdapter::LCDAdapter(std::shared_ptr<HD44780LCD> lcd)
        : _lcd(lcd)
    {        
    }

    LCDAdapter::~LCDAdapter()
    {
        spdlog::debug("LCDAdapter destroyed.");
    }

    void LCDAdapter::writeToPort(int portId, uint8_t data)
    {
        switch (portId) 
        {
            case DATA_PORT:
                _lcd->writeDataLines(data);
                break;
            case CONTROL_PORT:
                bool rs = (data & 0b00100000);
                bool rw = (data & 0b01000000);
                bool en = (data & 0b10000000);
                _lcd->setControlLines(rs, rw, en);
                break;
        }
    }
} // namespace EaterEmulator::devices