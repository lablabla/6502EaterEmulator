#pragma once

#include "core/bus_slave.h"

namespace EaterEmulator::devices
{
    // SRAM 62256 is a 32K x 8-bit SRAM
    class W65C22S : public core::BusSlave
    {
    public:
        W65C22S(core::Bus& bus);
        virtual ~W65C22S();

        W65C22S(const W65C22S&) = delete;
        W65C22S& operator=(const W65C22S&) = delete;

        W65C22S(W65C22S&&) = delete;
        W65C22S& operator=(W65C22S&&) = delete;
        
        void handleBusNotification(uint16_t address, uint8_t rwb) override;

        bool shouldHandleAddress(const uint16_t& address) const override;
        
        std::string getName() const override { return "W65C22S"; }

#ifdef UNIT_TEST

#endif 

    private:

    };
} // namespace EaterEmulator