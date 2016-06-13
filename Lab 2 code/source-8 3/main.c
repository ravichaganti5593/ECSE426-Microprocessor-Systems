#include <stdio.h>

#include "system_config.h"
#include "utils/util_timer.h"
#include "utils/lab2_util.h"
#include "utils/circular_buffer.h"
#include "ma_filter.h"
#include "temperature.h"
#include "alarm.h"
#include "led_interface.h"
#include "motor_interface.h"

/* This file contains a list of methods that are defined in the main method and SysTick_Handler
*/


extern float temperature_reading;
uint8_t system_ticks;


int main() {
	printf("Begin\n");

/* @param	system_init			initialize GPIO and ADC structs for pins and ADC variables using system_init()
*  @param	SysTick_Config		set ticks by dividing systemCoreClock with systick frequency (168MHz/50Hz)
*  @param	read_temperature() 	used to convert digital signal to temperature reading
*  @param	filtered			defined as moving average to reduce noise
*  @param	motor_rotate_to  	for the servo motor measurements (with filtered angle measurements)
*  @param	alarm_temperature 	defined to raise the alarm if the temperature goes above threshold
*/	
	system_init();

	SysTick_Config(SystemCoreClock / SYSTICK_FREQUENCY);
	//running while loop to control the frequency measurement readings of the temperature 
	//waits for interrupt to read the temperature readings
	while (1) {
		while (!system_ticks);
		
		read_temperature();

		float filtered = ma_filter(temperature_reading);
		motor_rotate_to(temperature_to_angle(filtered));
		alarm_temperature(filtered);

		system_ticks = 0;

		
	}
	
	
	return 0;
}


/*interrupt handler
*/

void SysTick_Handler(){
	system_ticks = 1;
}
