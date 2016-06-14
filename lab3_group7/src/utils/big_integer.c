#include "utils/big_integer.h"
#include "utils/utils.h"
#include "my_types.h"


void big_integer_increment(uint32_t* big_integer) {
	uint16_t index = 0;

	while (1) {
		if (big_integer[index] == MAX_UINT32_T - 1) {//Wrap around
			big_integer[index] = 0;
			index++;
		} else {
			big_integer[index]++;
			break;
		}
	}
}

void big_integer_decrement(uint32_t* big_integer);

void big_integer_add_const(uint32_t* big_integer, uint32_t adding_const) {
	uint16_t index = 0;

	//For example base 10, we add 8 + 5
	//8 > 9 - 5
	//5 - (9 - 8) = 4
	//--> last digit is 4 - 1 = 3
	//next digit is 1

	while (1) {
		if (big_integer[index] >= MAX_UINT32_T - adding_const) {
			adding_const -= MAX_UINT32_T - big_integer[index];
			big_integer[index] = adding_const - 1;
			adding_const = 1;
			index++;
		} else {
			big_integer[index] += adding_const;
			break;
		}
	}
}

static uint16_t first_non_zero(uint32_t* big_integer1, uint16_t big_integer1_len) {
	uint16_t index1 = big_integer1_len;

	while (1) {
		if (big_integer1[index1] != 0) {
			break;
		}

		if (index1 == 0) {
			break;
		}
		index1--;
	}
	return index1;
}

int big_integer_compare(uint32_t* big_integer1, uint16_t big_integer1_len, uint32_t* big_integer2, uint16_t big_integer2_len) {
	uint16_t index1 = first_non_zero(big_integer1, big_integer1_len);
	uint16_t index2 = first_non_zero(big_integer2, big_integer2_len);

	if (index1 > index2) {
		return 1;
	} else if (index1 < index2) {
		return -1;
	}
	
	while (1) {
		if (big_integer1[index1] == big_integer2[index2]) {
			if (index1 == 0 && index2 == 0) {
				return 0;
			} else {
				index1--;
				index2--;
			}
		} else if (big_integer1[index1] > big_integer2[index2]) {
			return 1;
		} else {
			return -1;
		}
	}
}
