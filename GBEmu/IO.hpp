#ifndef __IO_HPP__
#define __IO_HPP__

#include <iostream>

class IO {
public:
	IO() {};
	uint8_t readIO(uint16_t addr);
	void writeIO(uint16_t addr, uint8_t val);
private:
	static char serialData[2];
};

#endif