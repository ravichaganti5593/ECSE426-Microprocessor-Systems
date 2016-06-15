#ifndef ACCELEROMETER_SM_H
#define ACCELEROMETER_SM_H

#include "stm32f4xx.h"
#include "interfaces/accelerometer_interface.h"

/* accelerometer_info struct defined to take in parameters roll, pitch and yaw for tilting along x, y and z axes
*/

void accelerometer_calculate_angle(accelerometer_info* accelerometer_angles);

#endif
