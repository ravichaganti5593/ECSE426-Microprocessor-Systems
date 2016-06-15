#ifndef COMMANDS_H
#define COMMANDS_H

#include "stm32f4xx.h"

#define COMMAND_CLEAR_LEN 7
#define COMMAND_PLOT_LEN 7

extern const uint8_t CLEAR_COMMAND[] = {255, 255, 255, 255, 255, 255, 255};
extern const uint8_t PLOT_COMMAND[] = {32, 32, 32, 32, 32, 32, 32};

#endif
