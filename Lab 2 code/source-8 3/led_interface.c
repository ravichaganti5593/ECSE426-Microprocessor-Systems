#include "led_interface.h"
#include "stm32f4xx.h"                  // Device header

/*	led_interface.c implements 2 tasks: to turn on and off all the LEDs
*	@param	led_all_on 		will set all LEDs on (we haven't used this function)
*	@param	led_all_off 	will clear all LEDs by reseting all the GPIO pins of all the LEDs 
*/

static uint16_t leds[LED_COUNT] = {LED_1, LED_2, LED_3, LED_4};

void led_all_on() {
	for (uint8_t i = 0; i < LED_COUNT; i++) {
		SET_LED(leds[i]);
	}
}

void led_all_off() {
	for (uint8_t i = 0; i < LED_COUNT; i++) {
		CLEAR_LED(leds[i]);
	}
}
