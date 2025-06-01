#include "dbg.hpp"

void DBG::update(Bus& bus) {
	if (bus.read(0xFF02) == 0x81) { // Check if serial data is ready
		char c = bus.read(0xFF01); // Read serial data

		msg[msglgth++] = c;
		bus.write(0xFF02, 0); // Clear the serial control register
	}

}
void DBG::print() {
	if (msg[0])
	{
		std::cout << "DBG: " << msg;
	}
}