#pragma once

#include <cstdint>
#include <vector>

namespace EaterEmulator::core
{
    class BusSlave;

    class Bus
    {
    public:
        Bus() = default;
        virtual ~Bus() = default;

        // Non-copyable, movable
        Bus(const Bus&) = delete;
        Bus& operator=(const Bus&) = delete;
        Bus(Bus&&) = delete;
        Bus& operator=(Bus&&) = delete;
        
        void setAddress(uint16_t address);
        void getAddress(uint16_t& address) const;
        void setData(uint8_t data);
        void getData(uint8_t& data) const;

        void notifySlaves(uint8_t rwb) const;
        void addSlave(BusSlave* slave);
        

    private:
        uint16_t _address{0}; // Current address
        uint8_t _data{0}; // Current data

        std::vector<BusSlave*> _slaves; // List of slaves connected to this bus
    };
}