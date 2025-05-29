#ifndef __EMU_HPP__
#define __EMU_HPP__

#include <iostream>
#include "CPU.hpp"
#include "Bus.hpp"
#include "GBRomReader.hpp"

class Emu {
public:
	Emu(std::vector<uint8_t> memory) : bus(std::move(memory)), cpu() {};
	void run(bool d=false);
private:
	Bus bus;
	CPU cpu;
	bool debug;
};

#endif