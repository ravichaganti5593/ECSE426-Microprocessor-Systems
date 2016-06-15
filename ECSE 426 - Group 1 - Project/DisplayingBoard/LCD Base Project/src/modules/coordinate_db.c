#include <string.h>
#include "coordinate_db.h"

#include "my_types.h"

static COORDINATE_TYPE db[COORDINATE_DB_SIZE][2];
static uint8_t next_index;

/**
 * Initialize the module
 *
 * @param void
 *
 * @return void
 */
void coordinate_db_init(void) {
	next_index = 0;
}

/**
 * Clear the db content
 *
 * @param void
 *
 * @return void
 */
void coordinate_db_clear(void) {
	next_index = 0;
}

/**
 * Initialize the module
 *
 * @param input 		pointer to the buffer containing the coordinates in form of [x1, y1, x2, y2, ...]
 * @param input_len 	number of coordinates * 2 (i.e. twice number of pair (x,y))
 *
 * @return TRUE if success and FALSE otherwise
 */
uint8_t coordinate_db_insert_entry(COORDINATE_TYPE* input, uint8_t input_len) {
	if (next_index + (input_len / 2) >= COORDINATE_DB_SIZE) {
		return FALSE;
	}

	if (input_len == 0 || input_len % 2 != 0) {
		return FALSE;
	}

	memcpy((uint8_t*) &(db[next_index][0]), input, sizeof(COORDINATE_TYPE) * input_len);

	next_index += input_len / 2;
	return TRUE;
}

/**
 * Get the number of coordinates stored in the database
 *
 * @param void
 *
 * @return number of coordinates stored in the database
 */
uint8_t coordinate_db_get_len(void) {
	return next_index;
}

/**
 * Retrieve a single coordinate (x, y) from the database
 *
 * @param index 	index of the coordinate in the database 
 * @param result 	pointer to the result struct where (x, y) coordinate will be written to
 *
 * @return TRUE if retrieval succeed, and FALSE otherwise
 */
uint8_t coordinate_db_get_entry(uint8_t index, coordinate* result) {
	if (index < next_index) {
		memcpy((uint8_t*) result, &(db[index][0]), 2 * sizeof(COORDINATE_TYPE));
		return TRUE;
	}

	return FALSE;
}
