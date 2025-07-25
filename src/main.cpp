#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstdint>

#include "core/bus.h"

#include "devices/ArduinoMega/ArduinoMega.h"
#include "devices/EEPROM28C256/EEPROM28C256.h"
#include "devices/HD44780LCD/HD44780LCD.h"
#include "devices/HD44780LCD/LCDAdapter.h"
#include "devices/SRAM62256/SRAM62256.h"
#include "devices/W65C02S/CPUAdapter.h"
#include "devices/W65C02S/W65C02S.h"
#include "devices/W65C22S/W65C22S.h"
#include "spdlog/spdlog.h"

using namespace EaterEmulator;

int main(int argc, char* argv[]) {

    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] %v");
    spdlog::set_level(spdlog::level::info); 
    if (argc < 2) 
    {
        spdlog::error("No ROM file specified.");
        spdlog::info("Usage: {} <path_to_rom>", argv[0]);
        return 1;
    }

    std::vector<uint8_t> rom; // Buffer to hold the ROM data
    // Read the ROM into a buffer
    if(auto ifs = std::ifstream { argv[1], std::ios::binary }) 
    {
        auto size = std::filesystem::file_size(argv[1]);
        rom.resize(size);
        ifs.seekg(0, std::ios::beg);
        ifs.read(reinterpret_cast<char*>(rom.data()), rom.size());
    }
    else 
    {
        spdlog::error("Error reading ROM file: {}", argv[1]);
        return 1;
    }
    core::Bus bus;
    
    devices::W65C02S cpu6502(bus);
    cpu6502.reset();
    devices::EEPROM28C256 rom28C256(rom, bus);
    bus.addSlave(&rom28C256);
    devices::SRAM62256 ram62256(bus);
    bus.addSlave(&ram62256);
    devices::W65C22S w65c22s(bus);
    bus.addSlave(&w65c22s);
    // devices::ArduinoMega arduinoMega(bus);
    // bus.addSlave(&arduinoMega);

    auto lcd = std::make_shared<devices::HD44780LCD>();
    devices::LCDAdapter lcdAdapter(lcd);
    w65c22s.connect(devices::W65C22S::Port::A, lcdAdapter, devices::LCDAdapter::CONTROL_PORT);
    w65c22s.connect(devices::W65C22S::Port::B, lcdAdapter, devices::LCDAdapter::DATA_PORT);

    // devices::CPUAdapter cpuAdapter(std::make_shared<devices::W65C02S>(cpu6502));
    // w65c22s.connect(devices::W65C22S::Port::CB2, cpuAdapter, devices::CPUAdapter::IRQ_PORT);

    while (true)
    {
        cpu6502.handleClockStateChange(core::LOW);
        cpu6502.handleClockStateChange(core::HIGH);
    }
    
    return 0;
}