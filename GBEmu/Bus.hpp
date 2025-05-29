#ifndef __BUS_HPP__
#define __BUS_HPP__

#include <iostream>
#include <vector>

class Bus {
public:
	Bus(std::vector<uint8_t> mem) : memory(std::move(mem)) {};
	void write(uint16_t addr, uint8_t val);
	uint8_t read(uint16_t addr);

	uint16_t read16(uint16_t address);
	void write16(uint16_t address, uint16_t value);
private:
	std::vector<uint8_t> memory;
};


#endif // !__BUS_HPP__
