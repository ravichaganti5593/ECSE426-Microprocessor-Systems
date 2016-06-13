#ifndef UTIL_TIMER_H
#define UTIL_TIMER_H

#include "stm32f4xx.h"
#include "system_config.h"

#define ONE_SECOND_DELAY 18641351
#define ONE_MICRO_SECOND_DELAY 19

void timer_call_back(void);
int timer_count_down(uint32_t count);
int timer_count_expired(void);

int blocking_wait(uint32_t count);

#endif
