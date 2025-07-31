
#include "devices/SRAM62256/SRAM62256.h"

#include "core/bus.h"
#include "core/defines.h"
#include "spdlog/spdlog.h"
#include <sys/types.h>

namespace EaterEmulator::devices
{
    SRAM62256::SRAM62256(std::shared_ptr<core::Bus> bus) 
        : core::BusSlave(bus, 0x0000), _memory{}
    {
    }

    SRAM62256::~SRAM62256() 
    {
        spdlog::debug("SRAM62256 destroyed.");
    }

    void SRAM62256::handleBusNotification(uint16_t address, uint8_t rwb)
    {
        if (!shouldHandleAddress(address)) {
            spdlog::debug("SRAM62256: Address {:#04x} not handled by this device", address);
            return; // If the pins are not for this device, do nothing
        }
        if (rwb == core::HIGH)
        {
            // EEPROM Only handles when clock is HIGH
            _bus->setData(_memory[address - _offset]);
        }
        else
        {
            // Write operation
            uint8_t data;
            _bus->getData(data); // Get data from the bus
            _memory[address - _offset] = data; // Write data to the memory
            spdlog::debug("SRAM62256: Written data {:#04x} to address {:#04x}", data, address);
        }
    }

    bool SRAM62256::shouldHandleAddress(const uint16_t& address) const
    {
        // Decode address from the address bus and check if it falls within the range of this SRAM
        // SRAM is mapped to addresses 0x0000 to 0x3FFF so only if A14 and A15 are LOW
        return (address & (1 << 15)) == 0 && (address & (1 << 14)) == 0; 
    }
} // namespace EaterEmulator