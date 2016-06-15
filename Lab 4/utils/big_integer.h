#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include "stm32f4xx.h"

/*
* Implementation of big non-negative integer in base (2^32)
* This base is so big you will never use anything more than 3 digits, unless you are dealing with cryptographic business
*/
void big_integer_increment(uint32_t* big_integer);
void big_integer_decrement(uint32_t* big_integer);
void big_integer_add_const(uint32_t* big_integer, uint32_t adding_const);
int big_integer_compare(uint32_t* big_integer1, uint16_t big_integer1_len, uint32_t* big_integer2, uint16_t big_integer2_len);


#endif
