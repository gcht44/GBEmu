#include "IO.hpp"

uint8_t IO::readIO(uint16_t addr)
{
	switch (addr)
	{
		case 0xFF01: // Serial Data Register
			return serialData[0];
		case 0xFF02: // Serial Control Register
			return serialData[1];
		case 0xFF44: // LY Register
			return LY;
		default:
			std::cerr << "Read error: Invalid I/O address: " << std::hex << addr << std::dec << "\n";
			// exit(1);
			break;
	}
}
void IO::writeIO(uint16_t addr, uint8_t val)
{
	switch (addr)
	{
	case 0xFF01: // Serial Data Register
		serialData[0] = val;
	case 0xFF02: // Serial Control Register
		serialData[1] = val;
		break;
	default:
		std::cerr << "Write error: Invalid I/O address: " << std::hex << addr << std::dec << "\n";
		// exit(1);
		break;
	}
}