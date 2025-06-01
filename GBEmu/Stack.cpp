#include "Stack.hpp"

void Stack::push(uint8_t data, Register& reg, Bus& bus) {
    reg.SP--;
    bus.write(reg.SP, data);
}

void Stack::push16(uint16_t data, Register& reg, Bus& bus) {
    push((data >> 8) & 0xFF, reg, bus);
    push(data & 0xFF, reg, bus);
}

uint8_t Stack::pop(Register& reg, Bus& bus) {
    return bus.read(reg.SP++);
}

uint16_t Stack::pop16(Register& reg, Bus& bus) {
    uint16_t lo = pop(reg, bus);
    uint16_t hi = pop(reg, bus);

    return (hi << 8) | lo;
}