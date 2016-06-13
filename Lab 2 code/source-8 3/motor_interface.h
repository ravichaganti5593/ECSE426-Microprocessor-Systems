#ifndef MOTOR_INTERFACE_H
#define MOTOR_INTERFACE_H
#include "stm32f4xx_conf.h"

/*	black cable is connected to the ground
*	orange cable is connected to 5V input
*	@param 	MOTOR_PIN		yellow cable is connected to GPIOD_7 pin to calculate PWM signal and to convert it to angles for motor rotation
*/

#define MAX_ANGLE 180
#define MOTOR_PIN GPIO_Pin_7

extern float current_angle;

int motor_rotate_to(float angle);

#endif
