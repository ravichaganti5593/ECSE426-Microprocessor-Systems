#include "lab2_util.h"

/*	lab2_util.c implements the linear converion of temperature of the processor to the angle (on the analogue meter panel)
*	used equation y = mx + c to relate angle with temperature
*	angle = 4.25(temperature) - 80 (based on our manual calculations)
*/

float temperature_to_angle(float temperature) {
	static const float SLOPE = 4.25;
	static const float SHIFTED_CONST = -80;
	return temperature * SLOPE + SHIFTED_CONST;
}
