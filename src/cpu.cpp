#include "cpu.h"
#include <iostream>
#include <iomanip>

// Constructor: Initializes the CPU with a reference to memory and resets state
CPU::CPU(Memory& mem) : memory(mem) {
    reset();
}

// Reset: Clears all registers and flags, and resets the halted state
void CPU::reset() {
    for (int i = 0; i < ISA::COUNT; ++i) {
        registers[i] = 0;
    }
    halted = false;
}

uint8_t CPU::fetchByte() {
    return memory.read(registers[ISA::PC]++);
}

uint16_t CPU::fetchWord() {
    uint8_t low = fetchByte();
    uint8_t high = fetchByte();
    return (high << 8) | low;
}

void CPU::updateFlags(uint16_t result) {
    // Simple flag update: Zero flag only for now
    if (result == 0) {
        registers[ISA::FL] |= 0x01; // Set Zero flag
    } else {
        registers[ISA::FL] &= ~0x01; // Clear Zero flag
    }
}

bool CPU::step() {
    if (halted) return false;

    // 1. FETCH: Get the instruction opcode from memory at the current PC
    uint16_t pc = registers[ISA::PC];
    uint8_t opcode = fetchByte();

    // Debug output if enabled
    // ANSI Color Codes
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string BOLD = "\033[1m";

    if (debugMode) {
        std::cout << GREEN << "PC: " << std::hex << std::setw(4) << std::setfill('0') << pc << "   " << RESET;
    }

    // 2. DECODE & EXECUTE: Switch based on opcode to perform the operation
    switch (opcode) {
        case ISA::NOP:
            if (debugMode) std::cout << "IR: NOP" << std::endl;
            break;
        case ISA::HALT:
            if (debugMode) std::cout << RED << "IR: HALT" << RESET << std::endl;
            halted = true;
            return false;
        case ISA::MOVI: {
            uint8_t reg = fetchByte();
            uint16_t val = fetchWord();
            if (debugMode) {
                std::cout << BOLD << "IR: MOVI R" << (int)reg << ", " << std::hex << "0x" << val 
                          << "   R" << (int)reg << "=" << val << RESET << std::endl;
                std::cout << "Fetched: " << std::hex << (int)opcode << " " << (int)reg << " " << (int)(val & 0xFF) << " " << (int)(val >> 8) << std::endl;
                std::cout << BLUE << "Executed: R" << (int)reg << " <- " << val << RESET << std::endl;
            }
            if (reg < ISA::COUNT) registers[reg] = val;
            break;
        }
        case ISA::MOV: {
            uint8_t r1 = fetchByte();
            uint8_t r2 = fetchByte();
            if (debugMode) {
                std::cout << BOLD << "IR: MOV R" << (int)r1 << ", R" << (int)r2 
                          << "   R" << (int)r1 << "=" << registers[r2] << RESET << std::endl;
                 std::cout << "Fetched: " << std::hex << (int)opcode << " " << (int)r1 << " " << (int)r2 << std::endl;
                std::cout << BLUE << "Executed: R" << (int)r1 << " <- R" << (int)r2 << " (" << registers[r2] << ")" << RESET << std::endl;
            }
            if (r1 < ISA::COUNT && r2 < ISA::COUNT) registers[r1] = registers[r2];
            break;
        }
        case ISA::ADD: {
            uint8_t r1 = fetchByte();
            uint8_t r2 = fetchByte();
            if (debugMode) {
                std::cout << MAGENTA << "IR: ADD R" << (int)r1 << ", R" << (int)r2 << RESET << std::endl;
                std::cout << "Fetched: " << std::hex << (int)opcode << " " << (int)r1 << " " << (int)r2 << std::endl;
            }
            if (r1 < ISA::COUNT && r2 < ISA::COUNT) {
                registers[r1] += registers[r2];
                updateFlags(registers[r1]);
                if (debugMode) std::cout << MAGENTA << "Executed: R" << (int)r1 << " <- " << registers[r1] << RESET << std::endl;
            }
            break;
        }
        case ISA::SUB: {
            uint8_t r1 = fetchByte();
            uint8_t r2 = fetchByte();
            if (debugMode) {
                 std::cout << MAGENTA << "IR: SUB R" << (int)r1 << ", R" << (int)r2 << RESET << std::endl;
                 std::cout << "Fetched: " << std::hex << (int)opcode << " " << (int)r1 << " " << (int)r2 << std::endl;
            }
            if (r1 < ISA::COUNT && r2 < ISA::COUNT) {
                registers[r1] -= registers[r2];
                updateFlags(registers[r1]);
                 if (debugMode) std::cout << MAGENTA << "Executed: R" << (int)r1 << " <- " << registers[r1] << RESET << std::endl;
            }
            break;
        }
        case ISA::CMP: {
            uint8_t r1 = fetchByte();
            uint8_t r2 = fetchByte();
            if (debugMode) {
                std::cout << YELLOW << "IR: CMP R" << (int)r1 << ", R" << (int)r2 << RESET << std::endl;
                std::cout << "Fetched: " << std::hex << (int)opcode << " " << (int)r1 << " " << (int)r2 << std::endl;
            }
            if (r1 < ISA::COUNT && r2 < ISA::COUNT) {
                uint16_t res = registers[r1] - registers[r2];
                updateFlags(res);
                if (debugMode) std::cout << YELLOW << "Executed: CMP result=" << res << " ZeroFlag=" << ((registers[ISA::FL] & 1) ? "1" : "0") << RESET << std::endl;
            }
            break;
        }
        case ISA::JMP: {
            uint16_t addr = fetchWord();
            if (debugMode) {
                std::cout << YELLOW << "IR: JMP " << std::hex << addr << RESET << std::endl;
                std::cout << "Fetched: " << std::hex << (int)opcode << " " << (int)(addr & 0xFF) << " " << (int)(addr >> 8) << std::endl;
                std::cout << YELLOW << "Executed: PC <- " << addr << RESET << std::endl;
            }
            registers[ISA::PC] = addr;
            break;
        }
        case ISA::JZ: {
            uint16_t addr = fetchWord();
            if (debugMode) {
                std::cout << YELLOW << "IR: JZ " << std::hex << addr << RESET << std::endl;
                std::cout << "Fetched: " << std::hex << (int)opcode << " " << (int)(addr & 0xFF) << " " << (int)(addr >> 8) << std::endl;
            }
            if (registers[ISA::FL] & 0x01) {
                registers[ISA::PC] = addr;
                if (debugMode) std::cout << YELLOW << "Executed: PC <- " << addr << " (Taken)" << RESET << std::endl;
            } else {
                if (debugMode) std::cout << YELLOW << "Executed: Not Taken" << RESET << std::endl;
            }
            break;
        }
        case ISA::JNZ: {
            uint16_t addr = fetchWord();
            if (debugMode) {
                std::cout << YELLOW << "IR: JNZ " << std::hex << addr << RESET << std::endl;
                std::cout << "Fetched: " << std::hex << (int)opcode << " " << (int)(addr & 0xFF) << " " << (int)(addr >> 8) << std::endl;
            }
            if (!(registers[ISA::FL] & 0x01)) {
                registers[ISA::PC] = addr;
                if (debugMode) std::cout << YELLOW << "Executed: PC <- " << addr << " (Taken)" << RESET << std::endl;
            } else {
                if (debugMode) std::cout << YELLOW << "Executed: Not Taken" << RESET << std::endl;
            }
            break;
        }
        case ISA::OUT: {
            uint16_t port = fetchWord();
            uint8_t reg = fetchByte();
            if (debugMode) {
                std::cout << CYAN << "IR: OUT Port=" << port << ", R" << (int)reg << RESET << std::endl;
                std::cout << "Fetched: " << std::hex << (int)opcode << " " << (int)(port & 0xFF) << " " << (int)(port >> 8) << " " << (int)reg << std::endl;
            }
            if (reg < ISA::COUNT) {
                memory.write(port, (uint8_t)registers[reg]);
                if (debugMode) std::cout << CYAN << "Executed: Port[" << port << "] <- " << registers[reg] << RESET << std::endl;
            }
            break;
        }
        case ISA::LOAD: {
             uint8_t reg = fetchByte();
             uint16_t addr = fetchWord();
             if (debugMode) {
                 std::cout << BOLD << "IR: LOAD R" << (int)reg << ", [" << std::hex << addr << "]" << RESET << std::endl;
                 std::cout << "Fetched: " << std::hex << (int)opcode << " " << (int)reg << " " << (int)(addr & 0xFF) << " " << (int)(addr >> 8) << std::endl;
             }
             if (reg < ISA::COUNT) {
                 registers[reg] = memory.read(addr);
                 if (debugMode) std::cout << BLUE << "Executed: R" << (int)reg << " <- MEM[" << addr << "] (" << registers[reg] << ")" << RESET << std::endl;
             }
             break;
        }
        case ISA::STORE: {
            uint8_t reg = fetchByte();
            uint16_t addr = fetchWord();
            if (debugMode) {
                std::cout << BOLD << "IR: STORE R" << (int)reg << ", [" << std::hex << addr << "]" << RESET << std::endl;
                std::cout << "Fetched: " << std::hex << (int)opcode << " " << (int)reg << " " << (int)(addr & 0xFF) << " " << (int)(addr >> 8) << std::endl;
            }
            if (reg < ISA::COUNT) {
                memory.write(addr, (uint8_t)registers[reg]);
                if (debugMode) std::cout << BLUE << "Executed: MEM[" << addr << "] <- R" << (int)reg << " (" << registers[reg] << ")" << RESET << std::endl;
            }
            break;
        }
        default:
            std::cerr << RED << "Unknown Opcode: " << std::hex << (int)opcode << RESET << std::endl;
            halted = true;
            return false;
    }
    return true;
}

void CPU::dumpState() const {
    std::cout << "Registers:" << std::endl;
    for (int i = 0; i < ISA::COUNT; ++i) {
        std::cout << "R" << i << ": " << std::hex << std::setw(4) << std::setfill('0') << registers[i] << " ";
    }
    std::cout << std::endl;
}
