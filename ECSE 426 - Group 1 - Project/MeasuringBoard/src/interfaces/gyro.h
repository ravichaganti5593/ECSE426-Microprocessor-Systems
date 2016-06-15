#ifndef GYRO_H
#define GYRO_H

#include <stdbool.h>

/**
 * Flag indicating that the gyroscope interrupt has been triggered.
 */
extern bool gyro_interrupt;

/**
 * Initializes the gyroscope.
 */
void gyro_init(void);

/**
 * Gyroscope update function called by external line interrupt that gets
 * and stores the latest data from the gyroscope.
 */
void gyro_update(void);

/**
 * Gets the current angular velocity in the x direction. This value is filtered.
 *
 * @return The current angular velocity in the x direction.
 */
float gyro_get_x(void);

/**
 * Gets the current angular velocity in the y direction. This value is filtered.
 *
 * @return The current angular velocity in the y direction.
 */
float gyro_get_y(void);

/**
 * Gets the current angular velocity in the z direction. This value is filtered.
 *
 * @return The current angular velocity in the z direction.
 */
float gyro_get_z(void);

#endif // ifdef GYRO_H
