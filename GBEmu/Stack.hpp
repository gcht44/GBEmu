#ifndef __STACK_HPP__
#define __STACK_HPP__

#include <iostream>
#include "CPU.hpp"

class Stack {
public:
	Stack() {} ;
	void push(uint8_t data, CPU::Register& reg, Bus& bus);
	void push16(uint16_t data, CPU::Register& reg, Bus& bus);
	uint8_t pop(CPU::Register& reg, Bus& bus);
	uint16_t pop16(CPU::Register& reg, Bus& bus);
};

#endif