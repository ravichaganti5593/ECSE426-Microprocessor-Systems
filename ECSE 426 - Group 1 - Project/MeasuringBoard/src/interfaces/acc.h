#ifndef ACC_H
#define ACC_H

#include <stdbool.h>

/**
 * Flag indicating that the accelerometer interrupt has been triggered.
 */
extern bool acc_interrupt;

/**
 * Initializes the accelerometer.
 */
void acc_init(void);

/**
 * Accelerometer update function called by external line interrupt that gets
 * and stores the latest data from the accelerometer.
 */
void acc_update(void);

/**
 * Gets the current acceleration in the x direction. This value is filtered.
 *
 * @return The current acceleration in the x direction.
 */
float acc_get_x(void);

/**
 * Gets the current acceleration in the y direction. This value is filtered.
 *
 * @return The current acceleration in the y direction.
 */
float acc_get_y(void);

/**
 * Gets the current acceleration in the z direction. This value is filtered.
 *
 * @return The current acceleration in the z direction.
 */
float acc_get_z(void);

#endif // ifdef ACC_H
