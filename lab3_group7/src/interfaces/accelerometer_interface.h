#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "stm32f4xx.h"

/*
* Using NVIC with priority (0,0)
*/

#define GRAVITATIONAL_ACCELERATION     9.81

/* accelerometer_info struct defined to take in parameters roll, pitch and yaw for tilting along x, y and z axes
*/

typedef struct accelerometer_info {
	float roll;
	float pitch;
	float yaw;
} accelerometer_info;
extern accelerometer_info accelerometer_angles;

void accelerometer_init(void);
void accelerometer_read(float* x, float *y, float *z);
void accelerometer_calculate_rotation(float x, float y, float z);

#endif
