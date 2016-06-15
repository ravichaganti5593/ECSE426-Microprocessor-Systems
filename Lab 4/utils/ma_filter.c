#include "ma_filter.h"
#include "utils/circular_buffer.h"


/*	ma_filter.c implements the moving average filter to smoothen the temperature readings of the processor
*	ma_filter_initialize					will initialize the moving average filter's circular buffer
*	@param 	circular_buffer_size			will return the size of the filter buffer
*	@param	circular_buffer_append			will add values to the circular_buffer
*	@param	circular_buffer_remove_first	will remove the first element in the circular buffer
*/

void ma_filter_initialize(ma_filter* empty, circular_buffer* cb_data, MA_FILTER_TYPE* buffer) {
	circular_buffer_init(cb_data, buffer, MA_FILTER_DEPTH);
	empty->cb = cb_data;
	empty->average = 0;
}

float ma_filter_add(ma_filter* filter, MA_FILTER_TYPE input) {
	uint16_t size = 0;
	circular_buffer_size(filter->cb, &size);
	//if size is less than depth, then only size of the circular buffer will be used to calculate the average
	if (size < MA_FILTER_DEPTH) {
		float sum = (filter->average) * size + input;
		circular_buffer_append(filter->cb, &input);
		filter->average = sum / (size + 1);		//computing the average of the by taking the sum of elements in the filter depth and dividing it by depth continuously
		return filter->average;
	} else {									//if size is greater than depth, then depth will be used to calculate the moving average
		float sum = (filter->average) * MA_FILTER_DEPTH;
		MA_FILTER_TYPE removing = 1;
		circular_buffer_remove_first(filter->cb, &removing);
		circular_buffer_append(filter->cb, &input);
		sum -= removing;
		sum += input;
		filter->average = sum / (float)MA_FILTER_DEPTH;
		return filter->average;
	}
}

float ma_filter_get(ma_filter* filter) {
	return filter->average;
}
