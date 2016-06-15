#include "led_interface.h"
#include "stm32f4xx.h"                  // Device header

/*	led_interface.c implements 2 tasks: to turn on and off all the LEDs
*	@param	led_all_on 		will set all LEDs on (we haven't used this function)
*	@param	led_all_off 	will clear all LEDs by reseting all the GPIO pins of all the LEDs 
*/

static uint16_t leds[LED_COUNT] = {LED_1, LED_2, LED_3, LED_4};

void led_init(void) {
	GPIO_InitTypeDef gpio_init_s; // Structure to initilize definitions of GPIO
	GPIO_StructInit(&gpio_init_s); // Fills each GPIO_InitStruct member with its default value
	gpio_init_s.GPIO_Pin = LED_1 | LED_2 | LED_3 | LED_4; // Select the following pins to initialise
	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT; // Operating mode = output for the selected pins
	gpio_init_s.GPIO_Speed = GPIO_Speed_100MHz; // Don't limit slew rate, allow values to change as fast as they are set
	gpio_init_s.GPIO_OType = GPIO_OType_PP; // Operating output type (push-pull) for selected pins
	gpio_init_s.GPIO_PuPd = GPIO_PuPd_NOPULL; // If there is no input, don't pull.
	GPIO_Init(LED_GPIO, &gpio_init_s); // Initializes the GPIOD peripheral.
}

void led_all_on(void) {
	for (uint8_t i = 0; i < LED_COUNT; i++) {
		SET_LED(leds[i]);
	}
}

void led_all_off(void) {
	for (uint8_t i = 0; i < LED_COUNT; i++) {
		CLEAR_LED(leds[i]);
	}
}
