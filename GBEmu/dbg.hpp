#ifndef __DBG_HPP__
#define __DBG_HPP__

#include <iostream>
#include "Bus.hpp"

class DBG {
public:
	DBG() { msglgth = 0; };
	void update(Bus& bus);
	void print();
private:
	int msglgth;
	char msg[1024];
};

#endif