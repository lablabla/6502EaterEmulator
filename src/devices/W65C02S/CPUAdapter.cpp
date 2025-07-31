
#include "devices/W65C02S/CPUAdapter.h"
#include "devices/W65C02S/W65C02S.h"

#include "spdlog/spdlog.h"

namespace EaterEmulator::devices
{
    CPUAdapter::CPUAdapter(std::shared_ptr<W65C02S> cpu)
        : _cpu(cpu)
    {        
    }

    CPUAdapter::~CPUAdapter()
    {
        spdlog::debug("CPUAdapter destroyed.");
    }

    void CPUAdapter::writeToPort(int portId, uint8_t data)
    {
        switch (portId) 
        {
            case IRQ_PORT:
                _cpu->setIRQ(data == 0 ? core::LOW : core::HIGH);
                break;
        }
    }
} // namespace EaterEmulator::devices