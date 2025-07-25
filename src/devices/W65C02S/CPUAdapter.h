#pragma once

#include <memory>

namespace EaterEmulator::devices
{
    // Forward declarations
    class W65C02S;

    class CPUAdapter
    {
    public:
        static constexpr int IRQ_PORT = 0;

        CPUAdapter(W65C02S& cpu);
        ~CPUAdapter();

        CPUAdapter(const CPUAdapter&) = default;
        CPUAdapter& operator=(const CPUAdapter&) = delete;

        CPUAdapter(const CPUAdapter&&) = delete;
        CPUAdapter&& operator=(const CPUAdapter&&) = delete;

        void writeToPort(int portId, uint8_t data);

    private:
        W65C02S& _cpu;
    };
} // namespace EaterEmulator