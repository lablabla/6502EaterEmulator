
#include "devices/ArduinoMega/ArduinoMega.h"

#include "core/bus.h"
#include "core/defines.h"
#include "spdlog/spdlog.h"
#include <sys/types.h>

namespace EaterEmulator::devices
{
    ArduinoMega::ArduinoMega(core::Bus& bus) 
        : core::BusSlave(bus, 0x0000)
    {
        spdlog::info("ArduinoMega initialized.");
    }

    ArduinoMega::~ArduinoMega() 
    {
        spdlog::info("ArduinoMega destroyed.");
    }

    void ArduinoMega::handleBusNotification(uint16_t address, uint8_t rwb)
    {
        uint8_t data;
        _bus.getData(data);
        spdlog::info("{:016b}   {:04x}  {} {:02x}   {:08b}", address, address, rwb == core::HIGH ? 'r' : 'W', data, data);
    }

    bool ArduinoMega::shouldHandleAddress([[maybe_unused]]const uint16_t& address) const
    {
        return true;
    }
} // namespace EaterEmulator