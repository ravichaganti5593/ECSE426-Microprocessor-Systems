#ifndef KEYPAD_INPUT_SM_H
#define KEYPAD_INPUT_SM_H

#include "stm32f4xx.h"
#include "interfaces/keypad_interface.h"

#define KEYPAD_INPUT_NO_PRESS		KEYPAD_NO_PRESSED

#define KEYPAD_INPUT_POUND			17
#define KEYPAD_INPUT_STAR			18

uint8_t keypad_input_read(void);


#endif
