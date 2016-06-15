#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "map.h"

// Thresholds and counter max values for each turn type
#define LEFT_TURN_THRESHOLD 95000
#define PENDING_LEFT_TURN_COUNT 50
#define RIGHT_TURN_THRESHOLD -95000
#define PENDING_RIGHT_TURN_COUNT 50

// Thresholds and counter variables for each turn type
static bool pending_left_turn = false;
static int pending_left_turn_counter = 0;
static bool pending_right_turn = false;
static int pending_right_turn_counter = 0;

/**
 * Determine if a turn occurred given the latest gyroscope data.
 *
 * A left turn occurs when the x component of the gyroscope goes above a set
 * threshold, while a right turn occurs when the x component of the gyroscope
 * goes below a set threshold.
 *
 * @gyro_x The x component of the most recent gyroscope data.
 */
void turn_update(float gyro_x) {
    // Increment counters if pending turn has been registered
    if (pending_left_turn) {
        pending_left_turn_counter += 1;
    }
    if (pending_right_turn) {
        pending_right_turn_counter += 1;
    }
    
    // When threshold exceeded and enough time has passed since last turn,
    // register pending turn
    if (gyro_x > LEFT_TURN_THRESHOLD && pending_left_turn_counter == 0) {
        pending_left_turn = true;
    }
    if (gyro_x < RIGHT_TURN_THRESHOLD && pending_right_turn_counter == 0) {
        pending_right_turn = true;
    }
    
    // When enough time has passed after a pending turn was registered, tell
    // the map that the turn occurred and reset counters
    if (pending_left_turn_counter > PENDING_LEFT_TURN_COUNT) {
        map_turn_left();
        pending_left_turn_counter = 0;
        pending_left_turn = false;   
    }
    if (pending_right_turn_counter > PENDING_RIGHT_TURN_COUNT) {
        map_turn_right();
        pending_right_turn_counter = 0;
        pending_right_turn = false;   
    }
}
