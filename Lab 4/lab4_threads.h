#ifndef LAB4_THREADS_H
#define LAB4_THREADS_H

void lab4_os_init(void);
void thread_leds(void const* args);
void thread_temperature(void const* args);
void thread_keypad(void const* args);
void thread_accelerometer(void const* args);

#endif
