#include "modules/keypad_input_sm.h"
#include "interfaces/keypad_interface.h"

/*	keypad_input_read method will initialize the buttons on the keypad 
*	@param 			key_mapping			a 4X4 matrix to store all the buttons
*/

uint8_t keypad_input_read(void) {
	static const uint8_t key_mapping[4][4] = {
		{1, 2, 3, 0x0A},
		{4, 5, 6, 0x0B},
		{7, 8, 9, 0x0C},
		{KEYPAD_INPUT_STAR, 0, KEYPAD_INPUT_POUND, 0x0D}
	};

	uint8_t row, col;
	keypad_get_current_pressed_key(&row, &col);

	if (row != KEYPAD_INPUT_NO_PRESS && col != KEYPAD_INPUT_NO_PRESS) {
		if (row < 4 && col < 4) {
			return key_mapping[row][col];
		}
	}

	return KEYPAD_INPUT_NO_PRESS;
}
