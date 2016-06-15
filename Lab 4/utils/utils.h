#ifndef UTILS_H
#define UTILS_H

#include "stm32f4xx.h"

#ifndef M_PI
#define M_PI 3.1515926535898
#endif

#ifndef MAX_UINT32_T
#define MAX_UINT32_T 4294967295
#endif

float radian_to_degree(float rad);
uint16_t array_or(uint16_t* array, uint16_t length);
uint16_t array_or_const(const uint16_t* array, uint16_t length);

#endif
