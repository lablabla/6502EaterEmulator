#pragma once

#include <memory>

namespace EaterEmulator::devices
{
    // Forward declarations
    class HD44780LCD;

    class LCDAdapter
    {
    public:
        static constexpr int DATA_PORT = 0;
        static constexpr int CONTROL_PORT = 1;

        LCDAdapter(std::shared_ptr<HD44780LCD> lcd);
        LCDAdapter() = default;
        ~LCDAdapter();

        LCDAdapter(const LCDAdapter&) = default;
        LCDAdapter& operator=(const LCDAdapter&) = default;

        LCDAdapter(LCDAdapter&&) = default;
        LCDAdapter& operator=(LCDAdapter&&) = default;

        void writeToPort(int portId, uint8_t data);

    private:
        std::shared_ptr<HD44780LCD> _lcd = nullptr;
    };
} // namespace EaterEmulator
