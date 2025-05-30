#include "IO.hpp"

uint8_t IO::readIO(uint16_t addr)
{
	switch (addr)
	{
		if (addr == 0xFF01)
		{
			// Serial Data Register
			return serialData[0];
		}
		default:
			std::cerr << "Read error: Invalid I/O address: " << std::hex << addr << std::dec << "\n";
			exit(1);
			break;
	}
}
void IO::writeIO(uint16_t addr, uint8_t val)
{

}