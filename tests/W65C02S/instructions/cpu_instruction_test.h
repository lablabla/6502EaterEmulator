// Shared test fixture for 6502 CPU instruction tests
#pragma once
#include <gtest/gtest.h>

#include "spdlog/spdlog.h"

#include "core/clock.h"
#include "core/defines.h"
#include "devices/W65C02S/W65C02S.h"
#include "devices/EEPROM28C256/EEPROM28C256.h"


#include <memory>

using namespace EaterEmulator;

class CPUInstructionTest : public ::testing::Test {
protected:

    core::Bus bus;

    std::unique_ptr<devices::W65C02S> cpu;
    std::unique_ptr<devices::EEPROM28C256> rom;

    static constexpr uint16_t MEMORY_OFFSET = 0x8000; // Offset for the EEPROM memory
    
    std::vector<uint8_t> memory;
    void SetUp() override { 
        spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] %v");
        spdlog::set_level(spdlog::level::info);
        memory.resize(0x8000, 0x00);
        cpu = std::make_unique<devices::W65C02S>(bus);
        cpu->reset(); 
        cpu->setResetStage(2); // Simulate reset stage to avoid reset vector fetch
    }

    void TearDown() override {
        cpu.reset(); // Reset the CPU
        rom.reset(); // Reset the ROM
        memory.clear(); // Clear the memory vector
    }
};