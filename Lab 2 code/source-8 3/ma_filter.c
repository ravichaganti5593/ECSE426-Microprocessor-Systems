#include "ma_filter.h"
#include "utils/circular_buffer.h"

/*	ma_filter.c implements the moving average filter to smoothen the temperature readings of the processor
*	@param	ma_filter 						defined to calculate moving average filter using dynamic circular buffer
*	@param	initialize 						will initialize the circular_buffer
*	@param	circular_buffer_append			will add values to the circular_buffer
*	@param	circular_buffer_remove_first	will remove the first element in the circular buffer
*/

static float data_buffer[MA_FILTER_DEPTH + 2];
static float average = 0;
static uint8_t initialized = 0;
static circular_buffer cb_data;

void initialize(void) {
	circular_buffer_init(&cb_data, data_buffer, MA_FILTER_DEPTH);
}

float ma_filter(float input) {
	if (initialized == 0) {
		initialize();

		circular_buffer_append(&cb_data, &input);
		average = input;
		initialized = 1;
		return average;
	} else {
		uint16_t size = 0;
		circular_buffer_size(&cb_data, &size);
		//if size is less than depth, then only size of the circular buffer will be used to calculate the average
		if (size < MA_FILTER_DEPTH) {
			float sum = average * size + input;
			circular_buffer_append(&cb_data, &input);
			average = sum / (size + 1);		//computing the average of the by taking the sum of elements in the filter depth and dividing it by depth continuously
			return average;
		} else {
			float sum = average * MA_FILTER_DEPTH;
			float removing = 1;
			circular_buffer_remove_first(&cb_data, &removing);
			circular_buffer_append(&cb_data, &input);
			sum -= removing;
			sum += input;
			average = sum / MA_FILTER_DEPTH;
			return average;
		}
	}
}
