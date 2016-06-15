#ifndef TEMPERATURE_SENSOR_INTERFACE_H
#define TEMPERATURE_SENSOR_INTERFACE_H
#include "stm32f4xx.h"
#include "cmsis_os.h" // ARM::CMSIS:RTOS:Keil RTX

#define AVERAGE_SLOPE      2.5f
#define V_25               760

#define TEMPERATURE_SENSOR_POLL_PERIOD_MS    100

void temperature_sensor_init(void);
void temperature_sensor_set_semaphore(osSemaphoreId data_semaphore);
void temperature_sensor_read_temperature_raw(void);
void temperature_sensor_read(float* temperature);

#endif
