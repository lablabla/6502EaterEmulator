#pragma once

#include "core/bus_slave.h"
#include "core/defines.h"

#include "devices/HD44780LCD/LCDAdapter.h"
#include "devices/W65C02S/CPUAdapter.h"

#include <map>
#include <variant>

namespace EaterEmulator::devices
{
    using Peripherals = std::variant<LCDAdapter, CPUAdapter>;

    // VIA 6522 is a versatile I/O expander
    class W65C22S : public core::BusSlave
    {
    public:
        enum class Register
        {
            DATA_B = 0,
            DATA_A = 1,
            DDR_B = 2,
            DDR_A = 3,
            T1CL = 4,
            T1CH = 5,
            T1LL = 6,
            T1LH = 7,
            T2CL = 8,
            T2CH = 9,
            SR = 0xA,
            ACR = 0xB,
            PCR = 0xC,
            IFR = 0xD,
            IER = 0xE,
            DATA_A2 = 0xF // Alias for DATA_A, used in some operations
        };

        enum class Port
        {
            A,
            B,
            CB1,
            CB2,
            CA1,
            CA2
        };

        W65C22S(std::shared_ptr<core::Bus> bus);
        virtual ~W65C22S();

        W65C22S(const W65C22S&) = delete;
        W65C22S& operator=(const W65C22S&) = delete;

        W65C22S(W65C22S&&) = delete;
        W65C22S& operator=(W65C22S&&) = delete;
        
        void handleBusNotification(uint16_t address, uint8_t rwb) override;

        bool shouldHandleAddress(const uint16_t& address) const override;
        
        std::string getName() const override { return "W65C22S"; }

        void connect(Port viaPort, core::Peripheral auto device, int peripheralPortId)
        {
            connections[viaPort] = {device, peripheralPortId};
        }

    private:
        struct Connection {
            Peripherals device;
            int peripheralPortId;
        };
        std::map<Port, Connection> connections;
        bool handleRead(Register reg);
        bool handleWrite(Register reg);

        uint8_t readRegister(Register reg);
        uint8_t _dataA;
        uint8_t _dataB;
        uint8_t _ddrA;
        uint8_t _ddrB;
        uint8_t _t1cl;
        uint8_t _t1ch;
        uint8_t _t1ll;
        uint8_t _t1lh;
        uint8_t _t2cl;
        uint8_t _t2ch;
        uint8_t _sr;
        uint8_t _acr;
        uint8_t _pcr;
        uint8_t _ifr;
        uint8_t _ier;
    };
} // namespace EaterEmulator