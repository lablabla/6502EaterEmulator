#pragma once

#include "cpu/CPU.h"
#include "io/IODevice.h"

#include <memory>
#include <vector>

namespace EaterEmulator 
{
    class Computer 
    {
    public:
        Computer();
        virtual ~Computer() = default; // Default destructor

        Computer(const Computer&) = delete; // Disable copy constructor
        Computer& operator=(const Computer&) = delete; // Disable copy assignment
        Computer(Computer&&) = delete; // Disable move constructor
        Computer& operator=(Computer&&) = delete; // Disable move assignment

        void initDevices(const std::vector<uint8_t>& rom);

        void run();

    private:

        void addIODevice(std::shared_ptr<IODevice> device);
        

        DataBus _dataBus; // Pointer to the data bus
        AddressBus _addressBus; // Pointer to the address bus
        CPU _cpu; // Pointer to the CPU instance
        std::vector<std::shared_ptr<IODevice>> _devices; // List of I/O devices
    };
} // namespace EaterEmulator