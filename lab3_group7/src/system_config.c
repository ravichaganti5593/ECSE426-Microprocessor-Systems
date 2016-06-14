#include "system_config.h"
#include "stm32f4xx_conf.h"

#include "interfaces/accelerometer_interface.h"
#include "interfaces/led_interface.h"
#include "interfaces/seven_segments_interface.h"
#include "interfaces/keypad_interface.h"

#include "modules/seven_segments_sm.h"

/*	generic_init method will enable the clocks TIM2 and TIM3 the AHB1 peripheral clock to provide power to GPIOA, GPIOB and GPIOD (pins that are being used)
*/

static void generic_init(void) {
	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); // Enables the AHB1 peripheral clock, providing power to GPIOC branch
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE); // Enables the AHB1 peripheral clock, providing power to GPIOB branch
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOD, ENABLE); // Enables the AHB1 peripheral clock, providing power to GPIOD branch
	
	/* Enable clock for GPIOE */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	/* Enable clock for SYSCFG */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
}

/*	system_init method will initialize the following methods:
*	generic_init			to enable TIM clocks and to provide power to GPIO pins
*	led_init				to initialize pins GPIO_pin_12, 13, 14, 15 for LED display on the processor
*	seven_segment_init 		to initialize pins GPIOD_pin_0, 1, 2, 3, 4, 6, 7, 8 for seven-segment display for 1 digit and pins GPIOB_pin_4, 5, 6, 7 for all 4 digits
*	seven_segments_sm_init	to configure the TIM2 base and nested vector interrupt controller (NVIC)
*	keypad_init				to initialize the keypad configuration in terms of switching between input and output readings from keypad and configure TIM3 base and NVIC
*	accelerometer_init 		to configure x, y, z axes by enabling LIS3DSH (for C type processor) and its interrupt, GPIOE_pin_0, EXTI and NVIC to calculate angles of rotation
*/

void system_init(void) {
	generic_init();
	led_init();
	seven_segment_init();
	seven_segments_sm_init();
	keypad_init();
	accelerometer_init();
}

