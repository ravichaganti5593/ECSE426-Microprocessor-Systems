// #include <stdio.h>
// #include "system_config.h"

// #include "modules/led_rotation_sm.h"
// #include "modules/accelerometer_sm.h"
// #include "interfaces/seven_segments_interface.h"
// #include "interfaces/keypad_interface.h"

// static uint8_t system_ticks;

// int main() {
// 	printf("Begin\n");
// 	system_init();
// 	printf("Done system init\n");

// 	SysTick_Config(SystemCoreClock / SYSTICK_FREQUENCY);
// 	//running while loop to control the frequency measurement readings of the temperature 
// 	//waits for interrupt to read the temperature readings
// 	while (1) {
// 		while (!system_ticks);

// 		led_rotation_rotate_leds();
// 		accelerometer_process();
// 		keypad_read_state();
		
		
// 		static uint8_t x = 0, y = 0;
// 		static uint16_t count = 0;
		
// 		count++;
		
// 		if (count == 200) {
// 			x = (x + 1) % 16;
// 			count = 0;
// 		}

// 		static uint8_t is_1 = 0;
		
// 		if (is_1 == 0) {
// 			seven_segment_display(SEVEN_SEGMENT_DIGIT_1, x, x % 2);
// 		} else if (is_1 == 1) {
// 			seven_segment_display(SEVEN_SEGMENT_DIGIT_2, x, 1 - (x % 2));
// 		} else if (is_1 == 2) {
// 			seven_segment_display(SEVEN_SEGMENT_DIGIT_3, x, x % 2);
// 		} else {
// 			seven_segment_display(SEVEN_SEGMENT_DIGIT_4, x, 1 - (x % 2));
// 		}
// 		is_1 = (is_1 + 1) % 4;
		
// 		//seven_segment_display(SEVEN_SEGMENT_DIGIT_2, (x + 1) % 16);
// 		//seven_segment_display(SEVEN_SEGMENT_DIGIT_3, (x + 2) % 16);
// 		//seven_segment_display(SEVEN_SEGMENT_DIGIT_4, (x + 3) % 16);
		
		

// 		system_ticks = 0;
// 	}
	
	
// 	return 0;
// }


// /*interrupt handler
// */

// void SysTick_Handler() {
// 	system_ticks = 1;
// }
