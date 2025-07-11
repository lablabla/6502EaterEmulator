
#include "core/bus.h"
#include "core/bus_slave.h"
#include "spdlog/spdlog.h"

namespace EaterEmulator::core
{
    void Bus::setAddress(uint16_t address)
    {
        _address = address;
    }

    void Bus::getAddress(uint16_t& address) const
    {
        address = _address;
    }

    void Bus::setData(uint8_t data)
    {
        _data = data;
    }

    void Bus::getData(uint8_t& data) const
    {
        data = _data;
    }

    void Bus::addSlave(BusSlave* slave)
    {
        if (!slave) {
            spdlog::error("Bus: Attempted to add a null slave");
            return;
        }
        _slaves.push_back(slave);
        spdlog::debug("Bus: Slave {} added", slave->getName());
    }

    void Bus::notifySlaves(uint8_t rwb) const
    {
        for (const auto& slave : _slaves)
        {
            slave->handleBusNotification(_address, rwb);
        }
    }

};