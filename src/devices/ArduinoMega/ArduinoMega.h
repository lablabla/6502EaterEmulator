#pragma once

#include "core/bus_slave.h"
#include "spdlog/spdlog.h"
#include <memory>

namespace EaterEmulator::devices
{
    // Arduino Mega used for logging
    class ArduinoMega : public core::BusSlave
    {
    public:
        ArduinoMega(std::shared_ptr<core::Bus> bus);
        virtual ~ArduinoMega();

        ArduinoMega(const ArduinoMega&) = delete;
        ArduinoMega& operator=(const ArduinoMega&) = delete;

        ArduinoMega(ArduinoMega&&) = delete;
        ArduinoMega& operator=(ArduinoMega&&) = delete;
        
        void handleBusNotification(uint16_t address, uint8_t rwb) override;

        bool shouldHandleAddress(const uint16_t& address) const override;
        
        std::string getName() const override { return "ArduinoMega"; }

    private:
        std::shared_ptr<spdlog::logger> _logger;
    };
} // namespace EaterEmulator