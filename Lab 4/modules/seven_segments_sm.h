#ifndef SEVEN_SEGMENT_SM_H
#define SEVEN_SEGMENT_SM_H

#include "stm32f4xx.h"

#define SEVEN_SEGMENT_DISPLAY_PERIOD_MS		50
#define SEVEN_SEGMENT_BLINK_PERIOD          200 //Relative to display period

#define SEVEN_SEGMENT_DISPLAY_MODE_NORMAL   0
#define SEVEN_SEGMENT_DISPLAY_MODE_BLINK    1

#define SEVEN_SEGMENT_BASE_8				8
#define SEVEN_SEGMENT_BASE_10				10
#define SEVEN_SEGMENT_BASE_HEX				16

void seven_segments_sm_init(void);
void seven_segment_set_display_mode(uint8_t mode);
void seven_segment_set_display_degree(uint8_t is_on);

void seven_segment_set_display_int(uint16_t value, uint8_t base);
void seven_segment_set_display_float(float f, uint8_t decimal_places);
void seven_segments_set_display_float_smart(float f);
void seven_segment_periodic_display(void);

#endif
