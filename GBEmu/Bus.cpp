#include "Bus.hpp"


uint8_t Bus::read(uint16_t addr)
{
	if (addr < 0x4000)
	{
		// ROM bank 0
		return memory[addr];
	}
	else if (addr < 0x8000)
	{
		// ROM bank 1+
		uint16_t bankOffset = (addr - 0x4000) % 0x4000;
		uint8_t bankNumber = memory[0x147]; // Bank number from the header
		return memory[0x4000 * bankNumber + bankOffset];
	}
	else if (addr < 0xA000)
	{
		// VRAM
		std::cerr << "Read error: VRAM not implemented: " << std::hex << addr << std::dec << "\n";
		exit(1);
	}
	else if (addr < 0xC000)
	{
		std::cerr << "Read error: External RAM not implemented: " << std::hex << addr << std::dec << "\n";
		exit(1);
	}
	else if (addr < 0xE000)
	{
		// Work RAM
		return readRAM(addr);
	}
	else if (addr < 0xFE00)
	{
		// Echo RAM
		std::cerr << "Read error: Echo RAM not usable: " << std::hex << addr << std::dec << "\n";
		exit(1);
	}
	else if (addr < 0xFEA0)
	{
		// OAM
		return memory[addr];
	}
	else if (addr < 0xFF00)
	{
		// Unusable area
		std::cerr << "Read error: Unusable area: " << std::hex << addr << std::dec << "\n";
		exit(1);
	}
	else if (addr < 0xFF80)
	{
		// I/O ports
		return memory[addr];
	}
	else if (addr < 0xFFFF)
	{
		// High RAM
		return readHRAM(addr);
	}
	else if (addr == 0xFFFF)
	{
		// Interrupt Enable Register
		return readIE();
	}
	else
	{
		std::cerr << "Read error: Invalid address: " << std::hex << addr << std::dec << "\n";
		exit(1);
	}
	return 0;
}


void Bus::write(uint16_t addr, uint8_t val)
{
	if (addr < 0x4000)
	{
		// ROM bank 0
		memory[addr] = val;
	}
	else if (addr < 0x8000)
	{
		// ROM bank 1+
		uint16_t bankOffset = (addr - 0x4000) % 0x4000;
		uint8_t bankNumber = memory[0x147]; // Bank number from the header
		memory[0x4000 * bankNumber + bankOffset] = val;
	}
	else if (addr < 0xA000)
	{
		// VRAM
		std::cout << "Write error: VRAM not implemented: " << std::hex << addr << std::dec << "\n";
		exit(1);
	}
	else if (addr < 0xC000)
	{
		// External RAM
		std::cout << "Write error: External RAM not implemented: " << std::hex << addr << std::dec << "\n";
		exit(1);
	}
	else if (addr < 0xE000)
	{
		// Work RAM
		writeRAM(addr, val);
	}
	else if (addr < 0xFE00)
	{
		std::cerr << "Write error: Echo RAM not usable: " << std::hex << addr << std::dec << "\n";
		exit(1);
	}
	else if (addr < 0xFEA0)
	{
		memory[addr] = val; // OAM
	}
	else if (addr < 0xFF00)
	{
		std::cerr << "Write error: Unusable area: " << std::hex << addr << std::dec << "\n";
		exit(1);
	}
	else if (addr < 0xFF80)
	{
		memory[addr] = val; // I/O ports
	}
	else if (addr < 0xFFFF)
	{
		writeHRAM(addr, val); // High RAM
	}
	else if (addr == 0xFFFF)
	{
		writeIE(val); // Interrupt Enable Register
	}
	else
	{
		std::cerr << "Write error: Invalid address: " << std::hex << addr << std::dec << "\n";
		exit(1);
	}
}

uint16_t Bus::read16(uint16_t address) {
	uint16_t lo = read(address);
	uint16_t hi = read(address + 1);

	return lo | (hi << 8);
}

void Bus::write16(uint16_t address, uint16_t value) {
	write(address + 1, (value >> 8) & 0xFF);
	write(address, value & 0xFF);
}

void Bus::writeRAM(uint16_t addr, uint8_t val)
{
	addr -= 0xC000;

	RAM[addr] = val;
}
uint8_t Bus::readRAM(uint16_t addr)
{
	addr -= 0xC000;

	if (addr >= 0x2000) {
		std::cout << "INVALID RAM ADDR\n";
		exit(-1);
	}

	return RAM[addr];
}

void Bus::writeHRAM(uint16_t addr, uint8_t val)
{
	addr -= 0xFF80;
	HRAM[addr] = val;
}
uint8_t Bus:: readHRAM(uint16_t addr)
{
	addr -= 0xFF80;

	return HRAM[addr];
}

void Bus::writeIE(uint8_t val)
{
	IE = val;
}
uint8_t Bus::readIE()
{
	return IE;
}