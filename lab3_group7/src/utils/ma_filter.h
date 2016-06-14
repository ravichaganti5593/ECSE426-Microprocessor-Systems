#ifndef MA_FILTER_H
#define MA_FILTER_H

#include "stm32f4xx.h"
#include "circular_buffer.h"

#define MA_FILTER_DEPTH 14

typedef struct ma_filter {
	circular_buffer* cb;
	uint8_t initialized;
	float average;
} ma_filter;

void ma_filter_initialize(ma_filter* empty, circular_buffer* cb_data, float* buffer);
float ma_filter_add(ma_filter* filter, float input);

#endif
