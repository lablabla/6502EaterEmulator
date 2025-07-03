// Shared test fixture for 6502 CPU instruction tests
#pragma once
#include <gtest/gtest.h>
#include "cpu/CPU.h"
#include <array>

using namespace EaterEmulator;

class CPUInstructionTest : public ::testing::Test {
protected:
    std::array<uint8_t, 0x10000> memory{};
    std::shared_ptr<DataBus> dataBus = std::make_shared<DataBus>();
    std::shared_ptr<AddressBus> addressBus = std::make_shared<AddressBus>();
    CPU cpu{dataBus, addressBus};
    void SetUp() override { cpu.reset(); }
};
