#ifndef MAP_H
#define MAP_H

#include <stdint.h>

/**
 * Initialize the map.
 */
void map_init(void);

/**
 * Processes a left step by adding a coodinate to the map.
 */
void map_step_left(void);

/**
 * Processes a right step by adding a coodinate to the map.
 */
void map_step_right(void);

/**
 * Processes a left turn by changing the current orientation.
 */
void map_turn_left(void);

/**
 * Processes a right turn by changing the current orientation.
 */
void map_turn_right(void);

/** 
 * Gets the next set of coordinates for transmission by wireless.
 *
 * @param buf        The buffer into which to copy the coordinates.
 * @param length     The number of coordinates copied times 2.
 * @param max_length The maximum number of coordinates to copy times 2.
 *
 * @return 1 if some coordinates were copied, 0 if no coordinates were copied.
 */
uint8_t map_get_next_coordinates(int16_t *buf, uint16_t *length, 
    uint16_t max_length);

/**
 * Ignore any future calls to the map regarding registration of turns or steps.
 */
void map_terminate_processing(void);

#endif // MAP_H
