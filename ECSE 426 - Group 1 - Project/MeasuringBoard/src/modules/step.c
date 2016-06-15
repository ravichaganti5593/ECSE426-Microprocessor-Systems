#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "map.h"

// Thresholds and counter max values for each step type
#define PENDING_STEP_COUNT 50
#define LEFT_OR_RIGHT_STEP_THRESHOLD 1100
#define LEFT_STEP_THRESHOLD 600
#define FULL_STEP_THRESHOLD 1100

// Thresholds and counter variables for each step type
static bool pending_step = false;
static int pending_step_counter = 0;
static bool pending_step_left = false;
static bool pending_step_full = false;

/**
 * Determine if a step occurred given the latest accelerometer data.
 *
 * A right step occurs if a threshold is exceeded. A left step occurs if the 
 * left step threshold is exceeded and then another threshold is exceeded. Both
 * a right step and a left step occur if the left and right step thresholds are
 * exceeded and then another threshold is exceeded.
 *
 * @gyro_x The x component of the most recent accelerometer data.
 */
void step_update(float acc_x) {
    // Increment counters if pending step has been registered
    if (pending_step) {
        pending_step_counter += 1;
    }
    
    // When threshold exceeded and enough time has passed since last step,
    // register pending step
    if (acc_x > LEFT_OR_RIGHT_STEP_THRESHOLD && pending_step_counter == 0) {
        pending_step = true;
    }
    
    // Once enough time has passed, tell the map about the step type and reset 
    // counters
    if (pending_step_counter > PENDING_STEP_COUNT) {
        pending_step_counter = 0;
        if (pending_step_full) {
            map_step_right();
            map_step_left();
        } else if (pending_step_left) {
            map_step_left();
        } else {
            map_step_right();
        }
        pending_step_left = false;
        pending_step_full = false;
        pending_step = false;
    } else {
        // Register pending steps for steps with additional thresholds
        if (acc_x < LEFT_STEP_THRESHOLD) {
            pending_step_left = true;
        }
        if (acc_x > FULL_STEP_THRESHOLD && pending_step_left) {
            pending_step_full = true;
        }
    }
}
