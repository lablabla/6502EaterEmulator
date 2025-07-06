
#include "computer.h"

#include "devices/EEPROM28C256.h"
#include "devices/SRAM62256.h"
#include "devices/VIAW65C22.h"
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

    void Computer::initDevices(const std::vector<uint8_t>&rom) 
    {
        EEPROM28C256 eeprom(rom);
        addDevice(std::make_shared<EEPROM28C256>(eeprom));
        spdlog::info("EEPROM28C256 initialized.");

        // Add SRAM device
        addDevice(std::make_shared<SRAM62256>());
        spdlog::info("SRAM62256 initialized.");

        // Add VIA device
        addDevice(std::make_shared<VIAW65C22>());
        spdlog::info("VIAW65C22 initialized.");

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

    void Computer::addDevice(std::shared_ptr<Device> device) 
    {
        _devices.push_back(device);
        spdlog::info("Device added - {}.", device->getName());
    }

} // namespace EaterEmulator