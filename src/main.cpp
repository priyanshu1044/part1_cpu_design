#include <iostream>
#include <fstream>
#include <vector>
#include "cpu.h"
#include "memory.h"

    int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: cpu <program.bin> [--debug]" << std::endl;
        return 1;
    }

    bool debug = false;
    bool dumpMemory = false;
    if (argc >= 3) {
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--debug") {
                debug = true;
            } else if (arg == "--dump-memory") {
                dumpMemory = true;
            } else {
                std::cerr << "Unknown argument: " << arg << std::endl;
                return 1;
            }
        }
    }

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read((char*)buffer.data(), size)) {
        std::cerr << "Error reading file." << std::endl;
        return 1;
    }

    Memory mem;
    mem.loadProgram(buffer);

    CPU cpu(mem);
    cpu.setDebugMode(debug);

    std::cout << "Starting CPU..." << std::endl;
    
    while (cpu.step()) {
        // cpu.dumpState(); // Uncomment for debug trace
    }

    std::cout << "\nCPU Halted." << std::endl;
    cpu.dumpState();

    if (dumpMemory) {
        std::cout << "\nMemory Dump (First 64 bytes):" << std::endl;
        const auto& data = mem.getRawData();
        for (int i = 0; i < 64; ++i) {
            if (i % 16 == 0) {
                std::cout << std::hex << std::setw(4) << std::setfill('0') << i << ": ";
            }
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
            if ((i + 1) % 16 == 0) {
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
    }

    return 0;
}
