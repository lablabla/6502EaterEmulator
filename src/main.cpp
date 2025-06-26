#include "cpu/CPU.h"

#include <iostream>
#include <chrono>
#include <thread>

int main() 
{
    ya6502e::CPU cpu(nullptr, 0); // Create an instance of the CPU
    while (true) 
    {
        std::cout << "Ticking CPU..." << std::endl;
        cpu.tick(); // Execute one CPU cycle



        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Simulate a delay for each tick

    }
    return 0;
}