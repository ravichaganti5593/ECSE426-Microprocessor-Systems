#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include "stm32f4xx.h"                  // Device header

#define SYSTICK_FREQUENCY 50 //In Hz. So every tick is 1/(SYSTICK_FREQUENCY) seconds

void system_init(void);

#endif
