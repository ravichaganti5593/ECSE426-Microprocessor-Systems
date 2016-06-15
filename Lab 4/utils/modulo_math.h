#ifndef MODULO_MATH_H
#define MODULO_MATH_H

#include "stm32f4xx.h"

uint16_t modulo_add(uint16_t x, uint16_t y, uint16_t base);
uint16_t modulo_sub(uint16_t x, uint16_t y, uint16_t base);
int modulo_increment(uint16_t* x, uint16_t base);
int modulo_decrement(uint16_t* x, uint16_t base);

#endif
