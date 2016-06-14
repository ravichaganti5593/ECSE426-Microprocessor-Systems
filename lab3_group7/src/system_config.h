#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include "stm32f4xx.h"                  // Device header

#define SYSTICK_FREQUENCY 100 //In Hz. So every tick is 1/(SYSTICK_FREQUENCY) seconds
#define TIM_FREQUENCY     80500000 //Hz
#define TIM3_PRESCALER    TIM_FREQUENCY / 10000

void system_init(void);

#endif
