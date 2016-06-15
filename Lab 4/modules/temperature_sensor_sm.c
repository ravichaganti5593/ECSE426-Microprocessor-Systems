#include "temperature_sensor_sm.h"
#include "interfaces/temperature_sensor_interface.h"
#include "modules/seven_segments_sm.h"

/* temperature_sensor_sm.c will verify if the temperature is above threshold or not
*	 @param TEMPERATURE_ALARM_THRESHOLD 			defined as 40 degrees
*	 @param SEVEN_SEGMENT_DISPLAY_MODE_BLINK  defined as 1, will blink if temperature is greater than threshold
*	 @param SEVEN_SEGMENT_DISPLAY_MODE_NORMAL defined as 0, seven segment display will be stable and not blink values
*/

void temperature_sensor_alarm(float temperature) {
	if (temperature >= TEMPERATURE_ALARM_THRESHOLD) {
		//Alarm
		seven_segment_set_display_mode(SEVEN_SEGMENT_DISPLAY_MODE_BLINK);
	} else {
		//Normal
		seven_segment_set_display_mode(SEVEN_SEGMENT_DISPLAY_MODE_NORMAL);
	}
}
