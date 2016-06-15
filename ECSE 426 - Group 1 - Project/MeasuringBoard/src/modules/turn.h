#ifndef TURN_H
#define TURN_H

/**
 * Determine if a turn occurred given the latest gyroscope data.
 *
 * A left turn occurs when the x component of the gyroscope goes above a set
 * threshold, while a right turn occurs when the x component of the gyroscope
 * goes below a set threshold.
 *
 * @gyro_x The x component of the most recent gyroscope data.
 */
void turn_update(float gyro_x);

#endif // TURN_H
