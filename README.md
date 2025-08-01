[![CMake on multiple platforms](https://github.com/lablabla/6502EaterEmulator/actions/workflows/CI.yml/badge.svg)](https://github.com/lablabla/6502EaterEmulator/actions/workflows/CI.yml)

# 6502 Eater Emulator

A cycle-accurate 6502 computer emulator inspired by [Ben Eater's 6502 computer video series](https://www.youtube.com/playlist?list=PLowKtXNTBypFbtuVMUVXNR0z1mu7dp7eH) on YouTube. This emulator recreates the functionality of a 6502-based computer system with peripheral devices, providing a platform for learning, experimentation, and development.

## Table of Contents
- [About](#about)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Building](#building)
- [Running](#running)
- [Usage Examples](#usage-examples)
- [Project Structure](#project-structure)
- [Testing](#testing)
- [License](#license)

## About

This project implements a cycle-accurate emulation of the W65C02S microprocessor and associated peripheral devices, closely following the hardware design principles demonstrated in Ben Eater's educational video series. The emulator supports:

- **W65C02S CPU**: Full instruction set with cycle-accurate timing & interrupts support
- **Memory devices**: SRAM (62256) and EEPROM (28C256) emulation
- **Peripheral devices**: VIA (W65C22S), UART (W65C51N), and HD44780 LCD
- **Clock-phase accurate execution**: Separate handling of Φ2 low and high clock states
- **Extensible architecture**: Easy addition of new addressing modes and opcodes

## Features

- ✅ Complete 6502 instruction set implementation
- ✅ Cycle-accurate timing simulation
- ✅ Memory-mapped I/O support
- ✅ Extensible device architecture
- ✅ Assembly program loading and execution

## Prerequisites

### Required Dependencies
- **C++20 compatible compiler** (GCC 10+, Clang 12+)
- **CMake** 3.22 or higher
- **Ninja** build system (recommended)

### Optional Dependencies
- **vasm** assembler for building 6502 assembly programs


### Installing vasm (for assembly programs)
```bash
# Download and build vasm
wget http://sun.hasenbraten.de/vasm/release/vasm.tar.gz
tar -xzf vasm.tar.gz
cd vasm
make CPU=6502 SYNTAX=oldstyle
sudo cp vasm6502_oldstyle /usr/local/bin/
```

## Building

1. **Clone the repository**
   ```bash
   git clone https://github.com/lablabla/6502EaterEmulator.git
   cd 6502EaterEmulator
   ```

2. **Configure the build**
   ```bash
   mkdir build
   cd build
   cmake ..
   ```

3. **Build the project**
   ```bash
   cmake --build .

## Running

After building, the main executable will be located at `build/src/EaterEmulator`.

### Basic Usage
```bash
# Run the emulator
./build/src/EaterEmulator

# Run with specific ROM file
./build/src/EaterEmulator --rom path/to/program.bin

# Run with verbose logging
./build/src/EaterEmulator --verbose
```

## Usage Examples

### Running the Wozmon Program

The project includes Steve Wozniak's monitor program (Wozmon) as an example. Here's how to build and run it:

1. **Assemble the Wozmon source**
   ```bash
   # Navigate to test programs directory
   cd test_programs

   # Assemble wozmon.s to binary
   vasm6502_oldstyle -Fbin -dotdir -wdc02 -o wozmon.bin wozmon.s
   ```

2. **Run the emulator with Wozmon**
   ```bash
   # From the project root
   ./build/src/EaterEmulator --rom test_programs/wozmon.bin
   ```

3. **Interact with Wozmon**
   - The emulator will start executing the Wozmon program
   - Use the built-in commands to examine and modify memory
   - Example Wozmon commands:
     ```
     8000.8010    # Display memory from $8000 to $8010
     8000: A9 FF  # Store $A9 at $8000 and $FF at $8001
     8000G        # Jump to $8000 and execute
     ```

### Other Example Programs

```bash
# Run the hello world program
vasm6502_oldstyle -Fbin -dotdir -wdc02 -o hello-world.bin hello-world.s
./build/src/EaterEmulator --rom hello-world.bin

# Run load/store/jump test
./build/src/EaterEmulator test_programs/ld_st_jmp.bin
```

## Project Structure

```
6502EaterEmulator/
├── src/                          # Source code
│   ├── core/                     # Core emulator framework
│   │   ├── bus.cpp              # System bus implementation
│   │   ├── clock.h              # Clock definitions
│   │   └── device.h             # Base device interface
│   ├── devices/                 # Emulated devices
│   │   ├── W65C02S/            # 6502 CPU implementation
│   │   ├── SRAM62256/          # SRAM memory device
│   │   ├── EEPROM28C256/       # EEPROM memory device
│   │   ├── W65C22S/            # VIA peripheral
│   │   ├── W65C51N/            # UART peripheral
│   │   └── HD44780LCD/         # LCD display
│   └── main.cpp                # Main emulator entry point
├── tests/                       # Test suite
│   └── W65C02S/                # CPU instruction tests
├── test_programs/              # Example assembly programs
│   ├── wozmon.s               # Steve Wozniak's monitor
│   ├── hello-world.s          # Hello world example
│   └── ld_st_jmp.s           # Load/store/jump test
└── CMakeLists.txt             # CMake configuration
```

### Adding New Features
- **New Instructions**: Add opcode definitions to `opcodes.h` and implement handlers
- **New Devices**: Inherit from `core::Device` and implement required interfaces
- **New Addressing Modes**: Add to the addressing mode enum and implement handlers

## Testing

The project uses GoogleTest for comprehensive testing:

```bash
# Run all tests
cd build

# Run test executable directly
./tests/W65C02S/instructions/W65C02S_instruction_tests

# Run specific test cases
./tests/W65C02S/instructions/W65C02S_instruction_tests --gtest_filter="*ADC*"
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.