#include "alarm.h"
#include "led_interface.h"

/*	Alarm.c has the implementation of the alarm of the processor when the operating temperature crosses the threshold temperature
*	@param	alarm_temperature	method to define alarm for processor's temperature when it goes above threshold temperature
*	@param	led_all_off			will turn of all the LEDs
*	@param	SET_LED				set all LEDs to reset state initially if the temperature is less than threshold
*/

void alarm_temperature(float temperature) {
	static uint16_t state_led[] = {LED_1, LED_2, LED_3, LED_4};
	static uint8_t state = 0;
	static uint32_t count = 0;

	led_all_off();
	//if temperature is above threshold, set LED on 
	if (temperature > ALARM_THRESHOLD) {
		SET_LED(state_led[state]);
	}

	count++;
	//to make the LEDs light around in a circle, used a wrap-around indexing loop which will reset count
	if (count > 10) {
		state = (state + 1) % 4;
		count = 0;
	}
}
