#include <stdio.h>
#include "system_config.h"

#include "lab4_threads.h"
#include "cmsis_os.h" // ARM::CMSIS:RTOS:Keil RTX

/* main.c contains different 4 different threads for different operations that will be executed:
*	 thrdID1 				to display the LEDs
*	 thrdID2 				to call the temperature sensor methods to define the temperature of the processor
*  thrdID3 				to read inputs from keypad to display either roll/pitch angle or temperature of the processor
*  thrdID4 				to call the accelerometer sensor methods to define roll/pitch
*	 Priorities for all the threads are normal and defined by 0 
*/

osThreadId thrdID1;
void thread1(void const* args) {
	while (1) {
		thread_leds(args);
	}
}
osThreadDef(thread1, osPriorityNormal, 1, 100);         //stack size 100

osThreadId thrdID2;
void thread2(void const* args) {
	while (1) {
		thread_temperature(args);
	}
}
osThreadDef(thread2, osPriorityNormal, 1, 0);           //default stack size of 1000

osThreadId thrdID3;
void thread3(void const* args) {
	while (1) {
		thread_keypad(args);
	}
}
osThreadDef(thread3, osPriorityNormal, 1, 100);         //stack size 100

osThreadId thrdID4;
void thread4(void const* args) {
	while (1) {
		thread_accelerometer(args);
	}
}
osThreadDef(thread4, osPriorityNormal, 1, 0);           //default stack size of 1000

/* main method will initialize all the peripherals including keypad, acclerometer sensor, temperature sensor, seven segment display and GPIO pins
*	 
*/

int main() {
	printf("Begin\n");
	system_init();
	lab4_os_init();
	printf("Done system init\n");
	
	osKernelInitialize();								//will initialize RTOS kernel to create objects
	
	//4 threads are created 
	thrdID1 = osThreadCreate(osThread(thread1), NULL);
	thrdID2 = osThreadCreate(osThread(thread2), NULL);
	thrdID3 = osThreadCreate(osThread(thread3), NULL);
	thrdID4 = osThreadCreate(osThread(thread4), NULL);
	osKernelStart();                                    //will start the RTOS kernel
	
	return 0;
}
