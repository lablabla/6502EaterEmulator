
#include "devices/W65C22S/W65C22S.h"

#include "core/bus.h"
#include "core/defines.h"
#include "spdlog/spdlog.h"
#include <sys/types.h>

namespace EaterEmulator::devices
{
    W65C22S::W65C22S(core::Bus& bus) 
        : core::BusSlave(bus, 0x6000)
    {
    }

    W65C22S::~W65C22S() 
    {
        spdlog::debug("W65C22S destroyed.");
    }

    void W65C22S::handleBusNotification(uint16_t address, [[maybe_unused]]uint8_t rwb)
    {
        if (!shouldHandleAddress(address)) {
            spdlog::debug("W65C22S: Address {:#04x} not handled by this device", address);
            return; // If the pins are not for this device, do nothing
        }
    }

    bool W65C22S::shouldHandleAddress(const uint16_t& address) const
    {
        // Decode address from the address bus and check if it falls within the range of this VIA
        // VIA is mapped to addresses 0x6000 to 0x7FFF so only if A13 and A14 are HIGH and A15 is LOW
        return (address & (1 << 15)) == 0 && (address & (1 << 14)) != 0 && (address & (1 << 13)) != 0; 
    }
} // namespace EaterEmulator