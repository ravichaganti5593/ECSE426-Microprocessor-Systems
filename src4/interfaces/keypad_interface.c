#include <stdio.h>
#include "stm32f4xx_gpio.h"
#include "my_types.h"
#include "system_config.h"
#include "interfaces/keypad_interface.h"
#include "utils/utils.h"

/*	keypad_interface.c contains all the methods inlcuding input/output configuration, switch readings and the TIM3 hardware timer for switching delays
*	@param 		keypad_rows			an array which stores all the GPIOE pins for the 4 rows
*	@param 		keypad_cols			an array which stores all the GPIOE pins for the 4 columns
*/

static const uint16_t keypad_rows[KEYPAD_NUM_PINS_ROW] = {KEYPAD_ROW_1, KEYPAD_ROW_2, KEYPAD_ROW_3, KEYPAD_ROW_4};
static const uint16_t keypad_cols[KEYPAD_NUM_PINS_COL] = {KEYPAD_COL_1, KEYPAD_COL_2, KEYPAD_COL_3, KEYPAD_COL_4};

//static const uint16_t all_pins[KEYPAD_NUM_PINS_ROW * KEYPAD_NUM_PINS_COL] = {KEYPAD_PIN_1, KEYPAD_PIN_2, KEYPAD_PIN_3, KEYPAD_PIN_4, KEYPAD_PIN_5, KEYPAD_PIN_6, KEYPAD_PIN_7, KEYPAD_PIN_8};

static uint32_t keypad_time = 0, last_key_pressed = 0;
static uint8_t pressed_row = KEYPAD_NO_PRESSED, pressed_col = KEYPAD_NO_PRESSED;
static uint8_t is_reading_col = 0;
static uint8_t expected_pressed_value = 1;

#define KEYPAD_SET_GPIO(keypad_pin) GPIO_SetBits(KEYPAD_GPIO, keypad_pin)
#define KEYPAD_READ_GPIO(keypad_pin) GPIO_ReadInputDataBit(KEYPAD_GPIO, keypad_pin)

/*	configure_gpio_input method will initialize all the GPIO pins that are defined and set the mode as IN
*/

static void configure_gpio_input(GPIO_InitTypeDef* gpio_init_s, const uint16_t* pins, uint8_t number_of_pins) {
	GPIO_StructInit(gpio_init_s);
	gpio_init_s->GPIO_Pin = 0;
	for (uint8_t i = 0; i < number_of_pins; i++) {
		gpio_init_s->GPIO_Pin |= pins[i];
	}
	// printf("Calculated %d\n", gpio_init_s->GPIO_Pin);
	gpio_init_s->GPIO_Mode = GPIO_Mode_IN;
	gpio_init_s->GPIO_Speed = GPIO_Speed_100MHz;
	gpio_init_s->GPIO_OType = GPIO_OType_PP;
	gpio_init_s->GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(KEYPAD_GPIO, gpio_init_s);
}

/*	configure_gpio_output method will initialize all the GPIO pins that are defined and set the mode as OUT
*/

static void configure_gpio_output(GPIO_InitTypeDef* gpio_init_s, const uint16_t* pins, uint8_t number_of_pins) {
	GPIO_StructInit(gpio_init_s);
	gpio_init_s->GPIO_Pin = 0;
	for (uint8_t i = 0; i < number_of_pins; i++) {
		gpio_init_s->GPIO_Pin |= pins[i];
	}
	// printf("Calculated %d\n", gpio_init_s->GPIO_Pin);
	gpio_init_s->GPIO_Mode = GPIO_Mode_OUT;
	gpio_init_s->GPIO_Speed = GPIO_Speed_100MHz;
	gpio_init_s->GPIO_OType = GPIO_OType_PP;
	gpio_init_s->GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(KEYPAD_GPIO, gpio_init_s);
}

/*	switch_readings method will check if it's reading column or row as input or output
*	If it's reading column (set high as 1), then it will first configure the output by initializing pins and set the mode OUT and then configure the input by initializing pins and set the mode IN
*	If it's reading row, it will first configure the input by initializing the pins and set the mode IN 
*	We first configure the row as output and columns as input (with pull down enabled)
*/

static void switch_readings(void) {
	static GPIO_InitTypeDef gpio_init_s;

	if (is_reading_col == 1) {
		configure_gpio_output(&gpio_init_s, keypad_rows, KEYPAD_NUM_PINS_ROW);
		configure_gpio_input(&gpio_init_s, keypad_cols, KEYPAD_NUM_PINS_COL);

		KEYPAD_SET_GPIO(array_or_const(keypad_rows, KEYPAD_NUM_PINS_ROW));
	} else {
		configure_gpio_input(&gpio_init_s, keypad_rows, KEYPAD_NUM_PINS_ROW);
		configure_gpio_output(&gpio_init_s, keypad_cols, KEYPAD_NUM_PINS_COL);
		KEYPAD_SET_GPIO(array_or_const(keypad_cols, KEYPAD_NUM_PINS_COL));
	}

	is_reading_col = 1 - is_reading_col;
}

/*	keypad_init method will initialize the switch_readings method and the TIM3 hardware configuration for keypad interface
*	TIM_Period defined as 200-1 = 199ms
*/

void keypad_init(void) {
	switch_readings();

	/* Time base configuration */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = KEYPAD_POLL_PERIOD_MS - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = TIM3_PRESCALER - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	/* TIM IT enable */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);


	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the TIM3 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void submit_key_press(uint8_t found_row, uint8_t found_col) {
	if (found_row == KEYPAD_NO_PRESSED && found_col == KEYPAD_NO_PRESSED) {
		pressed_row = KEYPAD_NO_PRESSED;
		pressed_col = KEYPAD_NO_PRESSED;
	} else if (found_row != KEYPAD_NO_PRESSED && found_col != KEYPAD_NO_PRESSED) {
		if (keypad_time > last_key_pressed + KEYPAD_MIN_PRESS_DELAY) {
			pressed_row = found_row;
			pressed_col = found_col;
			last_key_pressed = keypad_time;
		}
	}
}

/* Once read, value will be removed. */
void keypad_get_current_pressed_key(uint8_t* row, uint8_t* col) {
	*row = pressed_row;
	*col = pressed_col;

	pressed_row = KEYPAD_NO_PRESSED;
	pressed_col = KEYPAD_NO_PRESSED;
}

void keypad_periodic_read_state(void) {
	keypad_time++;
	static uint8_t found_row = KEYPAD_NO_PRESSED, found_col = KEYPAD_NO_PRESSED;

	if (is_reading_col) {
		found_col = KEYPAD_NO_PRESSED;
		for (uint8_t i = 0; i < KEYPAD_NUM_PINS_COL; i++) {
			uint8_t pin_state = KEYPAD_READ_GPIO(keypad_rows[i]);
			if (pin_state == expected_pressed_value) {
				// printf("Col %d is pressed\n", i);
				found_col = i;
				break;
			}
		}

		// uint8_t row1 = KEYPAD_READ_GPIO(KEYPAD_ROW_1);
		// uint8_t row2 = KEYPAD_READ_GPIO(KEYPAD_ROW_2);
		// uint8_t row3 = KEYPAD_READ_GPIO(KEYPAD_ROW_3);
		// uint8_t row4 = KEYPAD_READ_GPIO(KEYPAD_ROW_4);

		// printf("Col are %d, %d, %d, %d\n", row1, row2, row3, row4);
	} else {
		found_row = KEYPAD_NO_PRESSED;
		for (uint8_t i = 0; i < KEYPAD_NUM_PINS_ROW; i++) {
			uint8_t pin_state = KEYPAD_READ_GPIO(keypad_cols[i]);
			if (pin_state == expected_pressed_value) {
				// printf("Row %d is pressed\n", i);
				found_row = i;
				break;
			}
		}

		// uint8_t col1 = KEYPAD_READ_GPIO(KEYPAD_COL_1);
		// uint8_t col2 = KEYPAD_READ_GPIO(KEYPAD_COL_2);
		// uint8_t col3 = KEYPAD_READ_GPIO(KEYPAD_COL_3);
		// uint8_t col4 = KEYPAD_READ_GPIO(KEYPAD_COL_4);

		// printf("Row are %d, %d, %d, %d\n", col1, col2, col3, col4);
	}

	submit_key_press(found_row, found_col);
	switch_readings();
}


void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		keypad_periodic_read_state();
	}
}
