#include <iostream>
#include <thread>
#include "core/bus.h"
#include "core/clock.h" 
#include "core/defines.h"

#include "devices/W65C02S/W65C02S.h"

using namespace EaterEmulator;

int main() {
    using namespace EaterEmulator::core;

    // Create a clock with a frequency of 1 MHz
    Clock<1> clock;
    core::Bus bus;
    // Create address and data buses
    devices::W65C02S cpu(bus);
    devices::W65C02S cpu2(bus);
    
    // Register CPU with clock
    clock.registerObserver(&cpu);
    clock.registerObserver(&cpu2);

    // Start the clock
    clock.start();

    std::cout << "Clock started. CPU running in background thread." << std::endl;

    // Simulate running for a while
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Stop the clock
    clock.stop();
    
    // Stop the CPU background thread
    cpu.stop();

    std::cout << "Clock and CPU stopped." << std::endl;
    return 0;
}