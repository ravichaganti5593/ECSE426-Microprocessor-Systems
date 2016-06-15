#include <stdio.h>
#include "cmsis_os.h"
#include "my_types.h"
#include "lab4_threads.h"

#include "interfaces/temperature_sensor_interface.h"
#include "interfaces/accelerometer_interface.h"

#include "modules/led_rotation_sm.h"
#include "modules/accelerometer_sm.h"
#include "modules/seven_segments_sm.h"
#include "modules/keypad_input_sm.h"
#include "modules/temperature_sensor_sm.h"

/*  lab4_threads.c will initialize the semaphore IDs created and run 4 threads that created for keypad, LEDs, accelerometer and temperature sensors
*/

static const uint8_t KEYPAD_DISPLAY_TEMPERATURE = 4;
static const uint8_t KEYPAD_DISPLAY_ACCELEROMETER = 5;
static const uint8_t KEYPAD_ACCELEROMETER_ROLL = 1;
static const uint8_t KEYPAD_ACCELEROMETER_PITCH = 2;

static const uint8_t DISPLAY_MODULE_TEMPERATURE = 0;
static const uint8_t DISPLAY_MODULE_ACCELEROMETER = 1;
static uint8_t display_module = DISPLAY_MODULE_TEMPERATURE;

static const uint8_t ACCELEROMETER_DISPLAY_ROLL = 0;
static const uint8_t ACCELEROMETER_DISPLAY_PITCH = 1;
static uint8_t accelerometer_display_mode = ACCELEROMETER_DISPLAY_ROLL;

/*******************************************************************************************************/
static osSemaphoreId lock_display_module, lock_accelerator_display_mode;			//name of semaphore objects, osSemaphoreId identifies the semaphore objects
static osSemaphoreId lock_temperature_data, lock_accelerometer_data;				

osSemaphoreDef(lock_display_module);												//defining names of semaphore objects
osSemaphoreDef(lock_accelerator_display_mode);
osSemaphoreDef(lock_temperature_data);
osSemaphoreDef(lock_accelerometer_data);

static uint8_t get_display_module(void) {
	uint8_t result;
	osSemaphoreWait(lock_display_module, osWaitForever);						//wait until a semaphore token is available
	result = display_module;													//will display either 0 or 1 depending on selecting temperature or accelerometer
	osSemaphoreRelease(lock_display_module);									//will release the semaphore name
	return result;
}

static void set_display_module(uint8_t value) {
	osSemaphoreWait(lock_display_module, osWaitForever);
	display_module = value;
	osSemaphoreRelease(lock_display_module);
}

static uint8_t get_accelerometer_display_mode(void) {
	uint8_t result;
	osSemaphoreWait(lock_accelerator_display_mode, osWaitForever);
	result = accelerometer_display_mode;
	osSemaphoreRelease(lock_accelerator_display_mode);
	return result;
}

static void set_accelerometer_display_mode(uint8_t value) {
	osSemaphoreWait(lock_accelerator_display_mode, osWaitForever);
	accelerometer_display_mode = value;
	osSemaphoreRelease(lock_accelerator_display_mode);
}

/*******************************************************************************************************/

////Create and Initialize lock_display_module, lock_accelerator_display_mode, lock_temperature_data, lock_accelerometer_data semaphore objects

void lab4_os_init(void) {
	lock_display_module = osSemaphoreCreate(osSemaphore(lock_display_module), 1);						
	lock_accelerator_display_mode = osSemaphoreCreate(osSemaphore(lock_accelerator_display_mode), 1);

	lock_temperature_data = osSemaphoreCreate(osSemaphore(lock_temperature_data), 1);
	temperature_sensor_set_semaphore(lock_temperature_data);

	lock_accelerometer_data = osSemaphoreCreate(osSemaphore(lock_accelerometer_data), 1);
	accelerometer_set_semaphore(lock_accelerometer_data);			//data will be passed to set the semaphore for accelerometer 
}



void thread_leds(void const* args) {
	led_rotation_rotate_leds();		//rotate LEDs 
	osDelay(20);				//in ms
}

void thread_temperature(void const* args) {
	osSemaphoreWait(lock_temperature_data, osWaitForever);		//Wait until a Semaphore token becomes available, in ms --> forever

	temperature_sensor_read_temperature_raw();					//will read the temperature values from the temperature sensor 
	static float temp = 1;
	temperature_sensor_read(&temp);

	if (get_display_module() == DISPLAY_MODULE_TEMPERATURE) {			//if 0 (for temperature to be displayed on the clock display)
		seven_segments_set_display_float_smart(temp);								//to display the decimal points 
		seven_segment_set_display_degree(TRUE);											//true is 1
	}

	temperature_sensor_alarm(temp);							//will call the alarm to check if the temperature is above threshold or not
}

void thread_keypad(void const* args) {
	uint8_t key_pressed = keypad_input_read();
	if (key_pressed == KEYPAD_ACCELEROMETER_ROLL) {									//if key pressed is 1, to display roll
		set_accelerometer_display_mode(ACCELEROMETER_DISPLAY_ROLL);		//display mode set to 0
	} else if (key_pressed == KEYPAD_ACCELEROMETER_PITCH) {					//if key pressed is 2, to display pitch
		set_accelerometer_display_mode(ACCELEROMETER_DISPLAY_PITCH);	//display mode set to 1
	} else if (key_pressed == KEYPAD_DISPLAY_TEMPERATURE) {					//if key pressed is 4, to display temperature
		set_display_module(DISPLAY_MODULE_TEMPERATURE);								//display mode set to 0
	} else if (key_pressed == KEYPAD_DISPLAY_ACCELEROMETER) {				//if key pressed is 5, to display either roll or pitch
		set_display_module(DISPLAY_MODULE_ACCELEROMETER);							//display mode set to 1 
	}

	osDelay(20);				//in ms
}

void thread_accelerometer(void const* args) {
	osSemaphoreWait(lock_accelerometer_data, osWaitForever);					//will lock accelerometer_data semaphore after semaphore is cleared

	accelerometer_read_raw();						//will read raw measurements from the accelerometer sensor
	static accelerometer_info angles;
	accelerometer_calculate_angle(&angles);		//will calculate angles for pitch and roll

	if (get_display_module() == DISPLAY_MODULE_ACCELEROMETER) {				//if mode 1 (to display accelerometer on the clock display)
		seven_segment_set_display_degree(TRUE);
		uint8_t display_mode = get_accelerometer_display_mode();
		if (display_mode == ACCELEROMETER_DISPLAY_ROLL) {								//if 0 is pressed to display roll
			seven_segments_set_display_float_smart(angles.roll < 0 ? -angles.roll : angles.roll);
		} else if (display_mode == ACCELEROMETER_DISPLAY_PITCH) {				//if 1 is pressed to display pitch
			seven_segments_set_display_float_smart(angles.pitch < 0 ? -angles.pitch : angles.pitch);
		}
	}
}
