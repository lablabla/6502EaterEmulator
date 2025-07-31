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

        CPUAdapter(std::shared_ptr<W65C02S> cpu);
        CPUAdapter() = default;
        ~CPUAdapter();

        CPUAdapter(const CPUAdapter&) = default;
        CPUAdapter& operator=(const CPUAdapter&) = default;

        CPUAdapter(const CPUAdapter&&) = delete;
        CPUAdapter&& operator=(const CPUAdapter&&) = delete;

        void writeToPort(int portId, uint8_t data);

    private:
        std::shared_ptr<W65C02S> _cpu = nullptr;
    };
} // namespace EaterEmulator