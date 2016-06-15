#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include "stm32f4xx.h"

/*	defined circular_buffer struct with values data, head, tail and max size
*/

#define CIRCULAR_BUFFER_TYPE float

typedef struct {
  CIRCULAR_BUFFER_TYPE* data;
  uint16_t head;
  uint16_t tail;
  uint16_t max_size;
} circular_buffer;

int circular_buffer_init(circular_buffer* empty, CIRCULAR_BUFFER_TYPE* data, uint16_t size);
int circular_buffer_is_full(circular_buffer* buffer);
int circular_buffer_is_empty(circular_buffer* buffer);

int circular_buffer_append(circular_buffer* buffer, CIRCULAR_BUFFER_TYPE* new_value);
int circular_buffer_remove_last(circular_buffer* buffer, CIRCULAR_BUFFER_TYPE* value);

int circular_buffer_get_first(circular_buffer* buffer, CIRCULAR_BUFFER_TYPE* value);
int circular_buffer_remove_first(circular_buffer* buffer, CIRCULAR_BUFFER_TYPE* value);

int circular_buffer_clear(circular_buffer* buffer);
int circular_buffer_size(circular_buffer* buffer, uint16_t* size);

int circular_buffer_get(circular_buffer* buffer, uint16_t index, CIRCULAR_BUFFER_TYPE* value);

#endif
