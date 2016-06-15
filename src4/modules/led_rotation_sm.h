#ifndef LED_ROTATION_SM_H
#define LED_ROTATION_SM_H

#include "stm32f4xx.h"

#define LED_ROTATION_MODE_ROTATE_SINGLE			0
#define LED_ROTATION_MODE_ROTATE_DOUBLE			1
#define LED_ROTATION_MODE_ROTATE_TRIPLE			2
#define LED_ROTATION_MODE_ROTATE_RACE			3
#define LED_ROTATION_MODE_BLINK					4

void led_rotation_set_mode(uint8_t mode);
void led_rotation_rotate_leds(void);

#endif
