#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "stm32f4xx.h"
#include "cmsis_os.h" // ARM::CMSIS:RTOS:Keil RTX

#define LSM9DS1

/*
* Using NVIC with priority (0,0)
*/

#define GRAVITATIONAL_ACCELERATION     9.81
typedef struct accelerometer_info {
	float roll;
	float pitch;
	float yaw;
} accelerometer_info;

void accelerometer_init(void);
void accelerometer_set_semaphore(osSemaphoreId data_semaphore);
void accelerometer_read_raw(void);
void accelerometer_read(float* x, float *y, float *z);
void accelerometer_calculate_rotation(accelerometer_info* accelerometer_angles, float x, float y, float z);

#endif
