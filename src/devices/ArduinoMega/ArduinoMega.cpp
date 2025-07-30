
#include "devices/ArduinoMega/ArduinoMega.h"

#include "core/bus.h"
#include "core/defines.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <sys/types.h>

namespace EaterEmulator::devices
{
    ArduinoMega::ArduinoMega(std::shared_ptr<core::Bus> bus) 
        : core::BusSlave(bus, 0x0000)
    {
        spdlog::debug("ArduinoMega initialized.");
        _logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt", true);
    }

    ArduinoMega::~ArduinoMega() 
    {
        spdlog::debug("ArduinoMega destroyed.");
    }

    void ArduinoMega::handleBusNotification(uint16_t address, uint8_t rwb)
    {
        uint8_t data;
        _bus->getData(data);
        _logger->info("{:016b}   {:04x}  {} {:02x}   {:08b}", address, address, rwb == core::HIGH ? 'r' : 'W', data, data);
        _logger->flush();
    }

    bool ArduinoMega::shouldHandleAddress([[maybe_unused]]const uint16_t& address) const
    {
        return true;
    }
} // namespace EaterEmulator