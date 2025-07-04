#include "computer.h"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>

int main(int argc, char* argv[]) 
{
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] %v");
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

    EaterEmulator::Computer computer; // Create an instance of the Computer
    computer.initDevices(rom); // Load the ROM into the computer
    spdlog::info("Starting the emulator...");
    computer.run(); // Run the emulator
    return 0;
}