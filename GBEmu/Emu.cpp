#include "Emu.hpp"

void Emu::run(bool d) {
	bool firstStep = true;
	debug = d;
	std::string cmd;
	int i = 1; // A supr

	while (cpu.isRunning()) {
		std::cout << "Step " << static_cast<int>(i++) << ": ";
		if (debug) {
			if (firstStep) { firstStep = false; }
			else
			{
				std::cout << "Enter command (next, exit): ";
				std::cin >> cmd;
				if (cmd == "exit") { return; }
				else if (cmd == "next") {}
				else {
					std::cout << "Unknown command: " << cmd << "\n";
					continue;
				}
			}
		}

		if (cpu.isHalted()) {
			continue;
		}
		uint8_t opcode = bus.read(cpu.readRegister(RT_PC));
		cpu.executeOpcode(opcode, bus);
	}
}