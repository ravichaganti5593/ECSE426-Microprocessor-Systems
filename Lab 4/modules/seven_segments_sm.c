#include <math.h>

#include "interfaces/seven_segments_interface.h"
#include "modules/seven_segments_sm.h"
#include "system_config.h"
#include "my_types.h"

/*	seven_segments_cm.c contains methods to display the digits on the clock display in different formats: XXX, XX.Y, X.YY
*/
static uint8_t digits[4];
static uint8_t dots[4];
static uint8_t display_mode = SEVEN_SEGMENT_DISPLAY_MODE_NORMAL;		//set to 0
static uint8_t display_degree = FALSE;		//0

/*	seven_segments_sm_init method will initialize the TIM2 hardware timer for the seven_segment display for each digit
*	TIM2 is passed on as a hardware interrupt request to the NVIC of the processor
*/

void seven_segments_sm_init(void) {
	/* Time base configuration */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = SEVEN_SEGMENT_DISPLAY_PERIOD_MS - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = TIM3_PRESCALER - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	/* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);


	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the TIM3 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void seven_segment_set_display_mode(uint8_t mode) {
	display_mode = mode;
}

void seven_segment_set_display_degree(uint8_t is_on) {
	display_degree = is_on;			//set to 1 to display degree
}

/*	to display seven_segment as an integer 
*/

void seven_segment_set_display_int(uint16_t value, uint8_t base) {
	if (base != SEVEN_SEGMENT_BASE_8 && base != SEVEN_SEGMENT_BASE_10 && base != SEVEN_SEGMENT_BASE_HEX) {
		return;
	}

	for (uint8_t i = 0; i < 4; i++) {
		digits[i] = value % base;
		value = value / base;

		dots[i] = 0;
	}
}

/*	seven_segment_set_display_float will display the seven_segment values in the form of decimals and will verify for the decimal places 
*/

void seven_segment_set_display_float(float f, uint8_t decimal_places) {
	f = f < 0 ? -f : f;

	for (uint8_t i = 0; i < 4; i++) {
		dots[i] = 0;
	}	

	if (decimal_places == 0) {
		seven_segment_set_display_int((uint16_t) floor(f), SEVEN_SEGMENT_BASE_10);
	} else if (decimal_places == 1) {
		seven_segment_set_display_int((uint16_t) floor(10*f), SEVEN_SEGMENT_BASE_10);
		dots[1] = 1;
	} else if (decimal_places == 2) {
		seven_segment_set_display_int((uint16_t) floor(100*f), SEVEN_SEGMENT_BASE_10);
		dots[2] = 1;
	}
	// else if (decimal_places == 3) {
	// 	seven_segment_set_display_int((uint16_t) floor(1000*f), SEVEN_SEGMENT_BASE_10);
	// 	dots[3] = 1;
	// }
}

void seven_segments_set_display_float_smart(float f) {
	if (f < 10) {
		seven_segment_set_display_float(f, 2);
	} else if (f < 100) {
		seven_segment_set_display_float(f, 1);
	} else {
		seven_segment_set_display_float(f, 0);
	}
}

void seven_segment_periodic_display(void) {
	static const uint16_t interface_digits[5] = {SEVEN_SEGMENT_DIGIT_4, SEVEN_SEGMENT_DIGIT_3, SEVEN_SEGMENT_DIGIT_2, SEVEN_SEGMENT_DIGIT_1, SEVEN_SEGMENT_DIGIT_DEGREE_SYMBOL};
	static uint8_t state = 0;
	static uint16_t blink_count;

	blink_count = (blink_count + 1) % SEVEN_SEGMENT_BLINK_PERIOD;       //blink period is 200
	state = (state + 1) % (display_degree == TRUE ? 4 : 3);             //if display_degree true, divide by 4, or else 3 for wrapping around

    
//if display_mode is set to blink (for overheating temperature of the processor)
	if (display_mode == SEVEN_SEGMENT_DISPLAY_MODE_BLINK && blink_count < SEVEN_SEGMENT_BLINK_PERIOD / 2) {
		seven_segment_display(SEVEN_SEGMENT_NO_DIGIT, 0, 0);        //will turn of all the digits
	} else {
		if (state != 3) {
			seven_segment_display(interface_digits[state + 1], digits[state], dots[state]); //will turn back the digits
		} else {
			seven_segment_display(SEVEN_SEGMENT_DIGIT_DEGREE_SYMBOL, 0, 0);
		}
	}
}


void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		seven_segment_periodic_display();
	}
}
