#include <stdio.h>
#include "modules/lab3_game.h"
#include "my_types.h"

#include "interfaces/led_interface.h"
#include "modules/accelerometer_sm.h"
#include "modules/seven_segments_sm.h"
#include "modules/keypad_input_sm.h"

/*	lab3_game.c contains methods which will enable user to input numbers from the keypad and display it on the clock display
*	@param 			buffer 		array defined to store the inputs given from the keypad
*/

void print_instruction(void) {
	printf("Guess the roll angle. Press * to enter...\n");
}

#define READ_INPUT		1
#define GUESS			2

static uint8_t buffer[100];
static uint16_t buffer_index = 0;

/*	get_user_guess method will initialize the buffer and store values and increment the index at the same time
*/

static uint16_t get_user_guess(void) {
	uint16_t value = 0;
	uint16_t index = 0;

	while (TRUE) {
		value += buffer[index];
		if (index == buffer_index) {
			break;
		}
		value *= 10;
		index++;
	}
	return value;
}
/*	user_press method will take in the value and store it in the next index 
*/

static void user_press(uint8_t value) {
	buffer_index++;
	buffer[buffer_index] = value;
}

/*	gg() method will start by setting game_over as false and constantly check if the game is over or not
*	@param		pressed 			input digit that is given by the user using the keypad
*	@param		GUESS 				once star is pressed on the keypad it will initiate the answer verifying process
*	@param		diff 				difference between the actual roll value and the user's guess
*	@param		LAB3_GAME_BOUND 	defined as 4 (which are the limits for the right guess)
*/

void gg(void) {
	static uint8_t game_over_correct = FALSE, game_over_incorrect = FALSE;
	static uint8_t state = READ_INPUT;
	static uint16_t try_left = LAB3_GAME_MAX_TRY;

	float angle = accelerometer_calculate_angle();	//method which will display the roll calculation of the accelerometer sensor

	angle = angle < 0 ? -angle : angle;			// to make sure that negative angles are displayed as positive angles 


	if (game_over_correct) {
		led_all_off();				
		SET_LED(LED_GREEN);													//will display green LED for correct guess
		seven_segments_set_display_float_smart(angle);						//will display the answer on the clock display with decimal points

	} else if (game_over_incorrect) {
		led_all_off();
		SET_LED(LED_ORANGE);
		seven_segments_set_display_float_smart(angle);
	} else {
		if (state == READ_INPUT) {											//if state == 1
			uint8_t pressed = keypad_input_read();							//keypad will read the input digit
			if (pressed == KEYPAD_INPUT_STAR) {								//	if star is pressed, that means the user has given his input

				state = GUESS;												//	will set the state to GUESS to loop out to the else statement

			} else if (pressed != KEYPAD_INPUT_NO_PRESS) {
				user_press(pressed);
			}
			seven_segment_set_display_int(get_user_guess(), SEVEN_SEGMENT_BASE_10);
		} else if (state == GUESS) {										// if state == 2 (enter the answer verifying procedure)
			float user_guess = (float) get_user_guess();
			buffer_index = 0;

			float diff = user_guess - angle;
			diff = diff < 0 ? -diff : diff;									//if difference is between 0 and -4, it is counted as positive difference
			printf("Value is %f\n", user_guess);
			if (diff < LAB3_GAME_BOUND) {
				game_over_correct = TRUE;									//set game_over_correct as true to exit the if/else and to display green LED. 
				printf("Guess correct!!!\n");
			} else {
				if (try_left == 0) {										//if tries left are 0, then game is over and is returned as true
					game_over_incorrect = TRUE;
				} else {
					state = READ_INPUT;
					try_left--;

					led_all_off();
					if (user_guess < angle) {
						printf("Less than expected\n");
						SET_LED(LED_RED);
					} else {
						printf("More than expected\n");
						SET_LED(LED_BLUE);
					}
				}
			}
		}
		
	}
}
