#include "circular_buffer.h"
#include "modulo_math.h"
#include "my_types.h"

/*	Used circular buffer to store values dynamically to calculate moving average to filter temperature reading measurements
*	circular_buffer_init 				will initalize the buffer 
*	circular_buffer_is_full 			will check if the buffer is full by checking if tail is equal to head by incrementing
*	circular_buffer_is_empty 			will check if the buffer is empty by checking head is equal to tail
*	circular_buffer_append 				add a new value in the buffer and increase the size by 1
*	circular_buffer_remove_last			remove the last element in the circular buffer and decrease the size by 1
*	circular_buffer_get_first 			will get the first element's data (which is the head)
*	circular_buffer_get_last 			will get last element's data
*	circular_buffer_add_first 			will add the value before the 1st element
*	circular_buffer_remove_first 		will remove the first element
*	circular_buffer_clear 				will clear the buffer of all the elements
*	circular_buffer_size 				will give the number of elements in the circular buffer
*	circular_buffer_get 				will get any element in the circular buffer
*	We expect data buffer to have at least size + 1 elements
*/

int circular_buffer_init(circular_buffer* empty, CIRCULAR_BUFFER_TYPE* data, uint16_t size) {
	if (size <= 1) {
		return -1;
	}

	empty->data = data;
	empty->head = 0;
	empty->tail = 0;
	empty->max_size = size;
	return TRUE;
}

int circular_buffer_is_full(circular_buffer* buffer) {
	uint16_t tail = buffer->tail;
	modulo_increment(&tail, buffer->max_size + 1);
	if (tail == buffer->head) {
		return TRUE;
	} else {
		return FALSE;
	}
}

int circular_buffer_is_empty(circular_buffer* buffer) {
	if (buffer->head == buffer->tail) {
		return TRUE;
	} else {
		return FALSE;
	}
}

int circular_buffer_append(circular_buffer* buffer, CIRCULAR_BUFFER_TYPE* new_value) {
	if (circular_buffer_is_full(buffer)) {
		return FALSE;
	}

	uint16_t* tail = &(buffer->tail);
	buffer->data[buffer->tail] = *new_value;
	modulo_increment(tail, buffer->max_size + 1);

	return TRUE;
}

int circular_buffer_remove_last(circular_buffer* buffer, CIRCULAR_BUFFER_TYPE* value) {
	if (circular_buffer_is_empty(buffer)) {
		return FALSE;
	}

	uint16_t* tail = &(buffer->tail);
	modulo_decrement(tail, buffer->max_size + 1);

	return TRUE;
}

int circular_buffer_get_first(circular_buffer* buffer, CIRCULAR_BUFFER_TYPE* value) {
	if (circular_buffer_is_empty(buffer)) {
		return FALSE;
	}

	*value = buffer->data[buffer->head];
	return TRUE;
}

int circular_buffer_get_last(circular_buffer* buffer, CIRCULAR_BUFFER_TYPE* value) {
	if (circular_buffer_is_empty(buffer)) {
		return FALSE;
	}

	uint16_t* index = &(buffer->tail);
	modulo_decrement(index, buffer->max_size + 1);
	*value = buffer->data[*index];
	return TRUE;
}

int circular_buffer_add_first(circular_buffer* buffer, CIRCULAR_BUFFER_TYPE* new_value) {
	if (circular_buffer_is_full(buffer)) {
		return FALSE;
	}

	uint16_t* head = &(buffer->head);
	modulo_decrement(head, buffer->max_size + 1);
	buffer->data[buffer->head] = *new_value;

	return TRUE;
}

int circular_buffer_remove_first(circular_buffer* buffer, CIRCULAR_BUFFER_TYPE* value) {
	if (circular_buffer_is_empty(buffer)) {
		return FALSE;
	}

	uint16_t* head = &(buffer->head);
	*value = buffer->data[buffer->head];
	modulo_increment(head, buffer->max_size + 1);

	return TRUE;
}

int circular_buffer_clear(circular_buffer* buffer) {
	return circular_buffer_init(buffer, buffer->data, buffer->max_size);
}

int circular_buffer_size(circular_buffer* buffer, uint16_t* size) {
	uint16_t head = buffer->head;
	uint16_t tail = buffer->tail;

	if (tail == head) {
		*size = 0;
	} else if (tail > head) {
		*size =  tail - head;
	} else {
		*size = buffer->max_size + 1 - (head - tail);
	}

	return TRUE;
}

int circular_buffer_get(circular_buffer* buffer, uint16_t index, CIRCULAR_BUFFER_TYPE* value) {
	uint16_t size = 0;
	circular_buffer_size(buffer, &size);
	if (index < size) {
		uint16_t real_index = (buffer->head + index) % (buffer->max_size);
		*value = (buffer->data)[real_index];
		return TRUE;
	} else {
		return FALSE;
	}
}
