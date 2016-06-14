#include "modulo_math.h"
#include "my_types.h"

/*	modulo_math.c implements basic addition, subtraction, decrement and increment of values around a base value (for wrapping around purposes)
*	@param 	modulo_add				add numbers and wrap around the base
*	@param	modulo_sub				subract numbers and wrap around the base
*	@param	modulo_increment		increment the number by 1 and wrap around the base
*	@param	modulo_decrement 		decrement the number by 1 and wrap around the base
*/

uint16_t modulo_add(uint16_t x, uint16_t y, uint16_t base) {
	return (x + y) % base;
}

uint16_t modulo_sub(uint16_t x, uint16_t y, uint16_t base) {
	if (x >= y) {
		return (x - y) % base;
	} else {
		return (x + base - y) % base;
	}
}

int modulo_increment(uint16_t* x, uint16_t base) {
	*x = ((*x) + 1) % base;
	return TRUE;
}

int modulo_decrement(uint16_t* x, uint16_t base) {
	*x = ((*x) - 1) % base;
	return TRUE;	
}
