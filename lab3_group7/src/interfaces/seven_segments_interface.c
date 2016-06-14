#include "seven_segments_interface.h"

/* seven_segments_interface.c contains methods for initializing the seven-segment display ports and pins and displaying the required digits that are measured from the accelerometer sensor
*  @param		all_digits		an array to store 4 GPIO pins for 4 digits on the clock display
*  @param		all_segments 	an array to store 7 GPIO pins for the seven segments (ABCDEFG) for each digit on the clock display and the GPIO pin for degree symbol
*/

static const uint16_t all_digits[SEVEN_SEGMENT_NUM_DIGIT] = {SEVEN_SEGMENT_DIGIT_1, SEVEN_SEGMENT_DIGIT_2, SEVEN_SEGMENT_DIGIT_3, SEVEN_SEGMENT_DIGIT_4, SEVEN_SEGMENT_DIGIT_DEGREE_SYMBOL};
static const uint16_t all_segments[SEVEN_SEGMENT_NUM_SEGMENT] = {SEVEN_SEGMENT_SEGMENT_A, SEVEN_SEGMENT_SEGMENT_B, SEVEN_SEGMENT_SEGMENT_C, SEVEN_SEGMENT_SEGMENT_D, SEVEN_SEGMENT_SEGMENT_E, SEVEN_SEGMENT_SEGMENT_F, SEVEN_SEGMENT_SEGMENT_G, SEVEN_SEGMENT_SEGMENT_DEGREE_SYMBOL};

/*	seven_segment_init method will initialize the GPIO pins for all the four digits and the seven-segment display for one digit
*/

void seven_segment_init(void) {
	GPIO_InitTypeDef gpio_init_s;
	GPIO_StructInit(&gpio_init_s);
	//The segments
	gpio_init_s.GPIO_Pin = 0;
	for (uint8_t i = 0; i < SEVEN_SEGMENT_NUM_SEGMENT; i++) {
		gpio_init_s.GPIO_Pin |= all_segments[i];			//initializing all the 7 pins for the seven segment display for ABCDEFG segments
	}
	gpio_init_s.GPIO_Pin |= SEVEN_SEGMENT_SEGMENT_DOT;
	gpio_init_s.GPIO_Pin |= SEVEN_SEGMENT_SEGMENT_DEGREE_SYMBOL;

	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init_s.GPIO_Speed = GPIO_Speed_100MHz;
	gpio_init_s.GPIO_OType = GPIO_OType_PP;
	gpio_init_s.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SEVEN_SEGMENT_SEGMENT_GPIO, &gpio_init_s);

	//The digits
	gpio_init_s.GPIO_Pin = 0;
	for (uint8_t i = 0; i < SEVEN_SEGMENT_NUM_DIGIT; i++) {
		gpio_init_s.GPIO_Pin |= all_digits[i];		//initializing all the 4 pins for all the 4 digits on the clock display

	}
	GPIO_Init(SEVEN_SEGMENT_DIGIT_GPIO, &gpio_init_s);
}

/*	choose_digit method will select one digit out of 4 digits and set it low and all the other digits high by resetting bits for that particular digit
*/

static void choose_digit(uint16_t digit) {
	//Configure the digit first by setting it to low, and everything else to high
	for (uint8_t i = 0; i < SEVEN_SEGMENT_NUM_DIGIT; i++) {
		if (all_digits[i] == digit) {
			GPIO_ResetBits(SEVEN_SEGMENT_DIGIT_GPIO, digit);
		} else {
			GPIO_SetBits(SEVEN_SEGMENT_DIGIT_GPIO, all_digits[i]);
		}
	}
}

/*	clear_segments method will reset all the pins of GPIOB on the processor to clear all the 4 digits on the clock display
*/

static void clear_segments(void) {
	//Clear all segments
	for (uint8_t i = 0; i < SEVEN_SEGMENT_NUM_SEGMENT; i++) {
		GPIO_ResetBits(SEVEN_SEGMENT_SEGMENT_GPIO, all_segments[i]);
	}
}

/*	seven_segments_clear_segments_at_digit will call the method choose_digit and clear_segments to clear all the 7 segments for 1 digit
*/

void seven_segments_clear_segments_at_digit(uint8_t digit) {
	choose_digit(digit);
	clear_segments();
}

/*	seven_segment_display method will highlight certain segments in a digit to display the required number 
*	@param 		values		a 16X7 matrix to display 16 outputs from 0,1,2,3,4,...F
*	@param		value 		to display values from 0,1,2,3,...F and will return nothing if greater than 15
*	@param 		dot_on		if dot_on is defined, then it will set GPIOD_Pin_8 on, else it will reset the pin
*/

void seven_segment_display(uint16_t digit, uint8_t value, uint8_t dot_on) {
	choose_digit(digit);
	clear_segments();
	
	if (digit == SEVEN_SEGMENT_DIGIT_DEGREE_SYMBOL) {
		GPIO_SetBits(SEVEN_SEGMENT_SEGMENT_GPIO, SEVEN_SEGMENT_SEGMENT_DEGREE_SYMBOL);
		return;
	}
	
	static const uint16_t values[16][SEVEN_SEGMENT_NUM_SEGMENT] = {
	//   A  B  C  D  E  F  G
		{1, 1, 1, 1, 1, 1, 0}, //0
		{0, 1, 1, 0, 0, 0, 0}, //1
		{1, 1, 0, 1, 1, 0, 1}, //2
		{1, 1, 1, 1, 0, 0, 1}, //3
		{0, 1, 1, 0 ,0 ,1, 1}, //4
		{1, 0, 1, 1 ,0 ,1, 1}, //5
		{1, 0, 1, 1 ,1 ,1, 1}, //6
		{1, 1, 1, 0 ,0 ,0, 0}, //7
		{1, 1, 1, 1 ,1 ,1, 1}, //8
		{1, 1, 1, 1 ,0 ,1, 1}, //9
		{1, 1, 1, 0 ,1 ,1, 1}, //10 - A
		{0, 0, 1, 1 ,1 ,1, 1}, //11 - B
		{1, 0, 0, 1, 1, 1, 0}, //12 - C
		{0, 1, 1, 1 ,1 ,0, 1}, //13 - D
		{1, 0, 0, 1 ,1 ,1, 1}, //14 - E
		{1, 0, 0, 0 ,1 ,1, 1}, //15 - F
	};

	if (value > 15) {
		return;
	}

	//Set the appropriate segment
	//will loop through all the 7 segments (ABCDEFG) in each row in the values matrix of size 16X7
	//will directly jump to the row which is defined by the value (0,1,2,...F)
	for (uint8_t i = 0; i < SEVEN_SEGMENT_NUM_SEGMENT; i++) {
		if (values[value][i] == 1) {
			GPIO_SetBits(SEVEN_SEGMENT_SEGMENT_GPIO, all_segments[i]);
		}
	}
	//to display the decimal point if dot_on parameter defined
	if (dot_on) {
		GPIO_SetBits(SEVEN_SEGMENT_SEGMENT_GPIO, SEVEN_SEGMENT_SEGMENT_DOT);
	} else {
		GPIO_ResetBits(SEVEN_SEGMENT_SEGMENT_GPIO, SEVEN_SEGMENT_SEGMENT_DOT);
	}
}
