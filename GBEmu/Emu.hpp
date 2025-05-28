#ifndef __EMU_HPP__
#define __EMU_HPP__

#include <iostream>
#include "CPU.hpp"
#include "Bus.hpp"
#include "GBRomReader.hpp"

class Emu {
public:
	Emu(std::vector<uint8_t> memory) : bus(std::move(memory)), cpu() {};
	void run() {
		while (cpu.isRunning()) {
			if (cpu.isHalted()) {
				continue;
			}
			uint8_t opcode = bus.read(cpu.readRegister16bit(CPU::RT_PC));
			cpu.executeOpcode(opcode, bus);
		}
	}
private:
	Bus bus;
	CPU cpu;
};

#endif