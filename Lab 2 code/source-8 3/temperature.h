#ifndef TEMPERAURE_H
#define TEMPERAURE_H
#include "stm32f4xx.h"

#define AVERAGE_SLOPE      2.5
#define V_25               760

extern float temperature_reading;
void read_temperature(void);

#endif
