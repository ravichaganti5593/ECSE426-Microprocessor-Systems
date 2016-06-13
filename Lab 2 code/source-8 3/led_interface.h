#ifndef LED_INTERFACE_H
#define LED_INTERFACE_H

#include "stm32f4xx.h"                  // Device header


/*	@param 	LED_1, LED2, LED3, LED_4 		define 3,4,5,6 on the processor with pins 12,13,14,15 for GPIOD 
*	@param 	SET_LED 						will set all the GPIO bits corresponding to the GPIO pin
*	@param  CLEAR_LED 						will reset all the GPIO bits corresponding to the GPIO pin
*/


#define LED_COUNT 4
#define LED_1 GPIO_Pin_12
#define LED_2 GPIO_Pin_13
#define LED_3 GPIO_Pin_14
#define LED_4 GPIO_Pin_15

#define SET_LED(id) GPIO_SetBits(GPIOD, id)
#define CLEAR_LED(id) GPIO_ResetBits(GPIOD, id)

void led_all_on(void);
void led_all_off(void);

#endif
