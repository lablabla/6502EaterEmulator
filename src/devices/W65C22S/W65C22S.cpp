
#include "devices/W65C22S/W65C22S.h"

#include "core/bus.h"
#include "core/defines.h"
#include "spdlog/spdlog.h"
#include <cstdint>
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

    void W65C22S::handleBusNotification(uint16_t address, uint8_t rwb)
    {
        if (!shouldHandleAddress(address)) {
            spdlog::debug("W65C22S: Address {:#04x} not handled by this device", address);
            return; // If the pins are not for this device, do nothing
        }

        auto reg = static_cast<Register>((address - _offset) & 0x0F);
        if (rwb == core::READ)
        {
            handleRead(reg);
        }
        else 
        {
            handleWrite(reg);            
        }
    }

    bool W65C22S::shouldHandleAddress(const uint16_t& address) const
    {
        // Decode address from the address bus and check if it falls within the range of this VIA
        // VIA is mapped to addresses 0x6000 to 0x7FFF so only if A13 and A14 are HIGH and A15 is LOW
        return (address & (1 << 15)) == 0 && (address & (1 << 14)) != 0 && (address & (1 << 13)) != 0; 
    }

    bool W65C22S::handleRead(Register reg)
    {
        uint8_t data = readRegister(reg);
        _bus.setData(data);
        return true;
    }

    bool W65C22S::handleWrite(Register reg)
    {
        uint8_t data;
        _bus.getData(data);
        switch(reg)
        {
            case Register::DATA_B:
                _dataB = data;
                writeData(Port::B);
                break;
            case Register::DATA_A:
            case Register::DATA_A2:
                _dataA = data;
                writeData(Port::A);
                break;
            case Register::DDR_B:
                _ddrB = data;
                writeData(Port::B);
                break;
            case Register::DDR_A:
                _ddrA = data;
                writeData(Port::A);
                break;
            case Register::T1CL:
            case Register::T1CH:
            case Register::T1LL:
            case Register::T1LH:
            case Register::T2CL:
            case Register::T2CH:
            case Register::SR:
            case Register::ACR:
            case Register::PCR:
            case Register::IFR:
            case Register::IER:
                break;
            default:
                spdlog::error("W65C22S: Invalid register for write operation: {:#04x}", static_cast<int>(reg));
                return false;
        }
        return true;
    }


    void W65C22S::writeData(Port port)
    {
        core::Bus& bus = (port == Port::A) ? _busA : _busB;
        uint8_t data = (port == Port::A) ? _dataA : _dataB;
        uint8_t ddr = (port == Port::A) ? _ddrA : _ddrB;

        uint8_t dataToWrite = data & ddr;
        bus.setData(dataToWrite);
        bus.notifySlaves(core::WRITE);
    }

    uint8_t W65C22S::readData(Port port)
    {
        const core::Bus& bus = (port == Port::A) ? _busA : _busB;
        uint8_t& data = (port == Port::A) ? _dataA : _dataB;
        uint8_t ddr = (port == Port::A) ? _ddrA : _ddrB;

        uint8_t dataToRead = 0;
        bus.notifySlaves(core::READ);
        bus.getData(dataToRead);

        data = dataToRead & ddr;
        return data;
    }

    uint8_t W65C22S::readRegister(Register reg)
    {
        switch (reg)
        {
            case Register::DATA_B: return _dataB;
            case Register::DATA_A: return _dataA;
            case Register::DDR_B: return _ddrB;
            case Register::DDR_A: return _ddrA;
            case Register::T1CL: return _t1cl;
            case Register::T1CH: return _t1ch;
            case Register::T1LL: return _t1ll;
            case Register::T1LH: return _t1lh;
            case Register::T2CL: return _t2cl;
            case Register::T2CH: return _t2ch;
            case Register::SR: return _sr;
            case Register::ACR: return _acr;
            case Register::PCR: return _pcr;
            case Register::IFR: return _ifr;
            case Register::IER: return _ier;
            case Register::DATA_A2: return _dataA;
            default:
                spdlog::error("W65C22S: Invalid register: {:#04x}", static_cast<int>(reg));
                return _dataA;
        }
    }
} // namespace EaterEmulator