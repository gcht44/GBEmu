#ifndef __BUS_HPP__
#define __BUS_HPP__

#include <iostream>
#include <vector>
#include "IO.hpp"

class Bus {
public:
	Bus(std::vector<uint8_t> mem) : memory(std::move(mem)), io(), RAM(0x2000) {}; // RAM(0x2000) initializes 8KB of RAM
	void write(uint16_t addr, uint8_t val);
	uint8_t read(uint16_t addr);

	uint16_t read16(uint16_t address);
	void write16(uint16_t address, uint16_t value);
private:
	std::vector<uint8_t> memory;
	std::vector<uint8_t> RAM;
	std::vector<uint8_t> HRAM;
	uint8_t IE; // Interrupt Enable Register
	IO io;

	void writeRAM(uint16_t addr, uint8_t val);
	uint8_t readRAM(uint16_t addr);
	void writeHRAM(uint16_t addr, uint8_t val);
	uint8_t readHRAM(uint16_t addr);

	void writeIE(uint8_t val);
	uint8_t readIE();
};


#endif // !__BUS_HPP__
