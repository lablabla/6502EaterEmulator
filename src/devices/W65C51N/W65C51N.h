#pragma once

#include "core/bus_slave.h"
#include "core/defines.h"

#include "devices/W65C02S/CPUAdapter.h"

#include <map>
#include <thread>
#include <variant>

namespace EaterEmulator::devices
{

    // W65C51N is a serial communication interface
    class W65C51N : public core::BusSlave
    {
    public:
        enum class Register
        {
            DATA,
            STATUS,
            COMMAND,
            CONTROL,
        };

        enum class Port
        {
            IRQ
        };

        W65C51N(std::shared_ptr<core::Bus> bus);
        virtual ~W65C51N();

        W65C51N(const W65C51N&) = delete;
        W65C51N& operator=(const W65C51N&) = delete;

        W65C51N(W65C51N&&) = delete;
        W65C51N& operator=(W65C51N&&) = delete;
        
        void handleBusNotification(uint16_t address, uint8_t rwb) override;

        bool shouldHandleAddress(const uint16_t& address) const override;
        
        std::string getName() const override { return "W65C51N"; }

        // void connect(Port viaPort, core::Peripheral auto device, int peripheralPortId)
        // {
        //     connections[viaPort] = {device, peripheralPortId};
        // }

    private:
        // struct Connection {
        //     Peripherals device;
        //     int peripheralPortId;
        // };
        // std::map<Port, Connection> connections;
        bool handleRead(Register reg);
        bool handleWrite(Register reg);

        uint8_t readRegister(Register reg);
        uint8_t _transmitData;
        uint8_t _status;
        uint8_t _command;
        uint8_t _control;
        uint8_t _receiveData;

        std::jthread _backgroundThread;
    };
} // namespace EaterEmulator