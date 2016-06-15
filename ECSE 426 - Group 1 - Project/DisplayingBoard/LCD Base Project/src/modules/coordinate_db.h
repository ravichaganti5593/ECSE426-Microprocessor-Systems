#ifndef COORDINATE_DB_H
#define COORDINATE_DB_H

#include "stm32f4xx.h"

#define COORDINATE_DB_SIZE 250
#define COORDINATE_SIZE    2

//Datatype used to present one coordinate item (either x or y)
typedef int16_t COORDINATE_TYPE;

//Definition of data structure of a coordinate
typedef struct {
	COORDINATE_TYPE x;
	COORDINATE_TYPE y;
} coordinate;

/**
 * Initialize the module
 *
 * @param input 		pointer to the buffer containing the coordinates in form of [x1, y1, x2, y2, ...]
 * @param input_len 	number of coordinates * 2 (i.e. twice number of pair (x,y))
 *
 * @return TRUE if success and FALSE otherwise
 */
void coordinate_db_init(void);

/**
 * Clear the db content
 *
 * @param void
 *
 * @return void
 */
void coordinate_db_clear(void);

/**
 * Initialize the module
 *
 * @param input 		pointer to the buffer containing the coordinates in form of [x1, y1, x2, y2, ...]
 * @param input_len 	number of coordinates * 2 (i.e. twice number of pair (x,y))
 *
 * @return TRUE if success and FALSE otherwise
 */
uint8_t coordinate_db_insert_entry(COORDINATE_TYPE* input, uint8_t input_len);

/**
 * Get the number of coordinates stored in the database
 *
 * @param void
 *
 * @return number of coordinates stored in the database
 */
uint8_t coordinate_db_get_len(void);

/**
 * Retrieve a single coordinate (x, y) from the database
 *
 * @param index 	index of the coordinate in the database 
 * @param result 	pointer to the result struct where (x, y) coordinate will be written to
 *
 * @return TRUE if retrieval succeed, and FALSE otherwise
 */
uint8_t coordinate_db_get_entry(uint8_t index, coordinate* result);


#endif
