
#include "computer.h"

#include "io/EEPROM28C256.h"
#include "spdlog/spdlog.h"

namespace EaterEmulator 
{
    Computer::Computer() 
        : 
            _dataBus(), 
            _addressBus(), 
            _cpu(_dataBus, _addressBus),
            _devices()
    {
        spdlog::info("Computer initialized.");
    }

    void Computer::addRom(const std::vector<uint8_t>&rom) 
    {
        EEPROM28C256 eeprom(rom);
        addIODevice(std::make_shared<EEPROM28C256>(eeprom));
        spdlog::info("ROM loaded.");
    }

    void Computer::run() 
    {
        _cpu.setDevices(_devices); // Set the devices for the CPU
        _cpu.reset(); // Reset the CPU state
        while (true) 
        {
            _cpu.runNextInstruction();
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Simulate a delay for each tick
        }
    }

    void Computer::addIODevice(std::shared_ptr<IODevice> device) 
    {
        _devices.push_back(device);
        spdlog::info("IODevice added - {}.", device->getName());
    }

} // namespace EaterEmulator