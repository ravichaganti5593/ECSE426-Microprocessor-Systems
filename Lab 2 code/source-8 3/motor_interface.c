#include "motor_interface.h"
#include "utils/my_types.h"
#include "utils/util_timer.h"

/*	motor_interface.c implements the PWM signal theory to enable motor rotation from 0 to 180 degrees to plot the temperature of the processor. 
*/


float current_angle;

int motor_rotate_to(float angle) {
	//if angle is less than 0 or greater than 180 then no operation of rotation will be performed
	if (angle < 0 || angle >= MAX_ANGLE) {
		return FALSE;
	}

	GPIO_SetBits(GPIOD, MOTOR_PIN);

/*	@param	_0_DEGREE 		470us corresponds to the lowest possible range to define 0 degrees 
*	@param	_180_DEGREE		2200us corresponds to the highest possible range to define 180 degrees
*/
	static const uint32_t _0_DEGREE = 470;
	static const uint32_t _180_DEGREE = 2200;
	
/*	@param	wait_time 		defined with respect to angle rotation (which is defined in us) (using y = mx + c where c is 470, m = 2200-470/180)
*	@param	blocking_wait 	will take over the processor system for a duration of wait_time*1us delay and nop() is called
*/
	uint32_t wait_time = (uint32_t)(_0_DEGREE + (angle / (float)MAX_ANGLE) * (_180_DEGREE - _0_DEGREE));
	
	blocking_wait(wait_time * ONE_MICRO_SECOND_DELAY);
	//after the operation is over it will reset GPIOD_7
	GPIO_ResetBits(GPIOD, MOTOR_PIN);

	return TRUE;
}
