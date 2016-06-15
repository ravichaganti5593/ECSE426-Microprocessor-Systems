#include <stdbool.h>
#include <math.h>

#include "stm32f4xx.h"
#include "../drivers/lsm9ds1.h"

#include "gyro.h"

/**
 * Gyroscope data and constants
 */

#define AXES_COUNT 3

typedef enum {
    AXIS_X,
    AXIS_Y,
    AXIS_Z
} axes_type;

// Calibration matrices used to calibrate the gyroscope readings. The
// matrices were derived from a 3 variable least-squares approximation on test
// data
static float calibration_matrix[AXES_COUNT][AXES_COUNT] = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
};
static float calibration_offset_matrix[AXES_COUNT] = {
    -600,
    -2200,
    650
};


#define GYRO_TIM               TIM3
#define GYRO_TIM_RCC           RCC_APB1Periph_TIM3

/**
 * The timer frequency is 120 Hz.
 *
 * The following formula was used to calculate the prescaler and period to get
 * this frequency:
 *
 * 120 Hz = 84000000 Hz / (prescaler * period)
 *
 * 5 and 56000 happens to be one integer solution to this formula, but others
 * would work.
 */
#define GYRO_TIM_PRESCALER     (uint16_t) 15
#define GYRO_TIM_PERIOD        (uint16_t) 56000

#define GYRO_NVIC_CHANNEL      TIM3_IRQn
#define GYRO_NVIC_PRIORITY     (uint8_t) 1
#define GYRO_NVIC_SUBPRIORITY  (uint8_t) 0

// Flag indicating that the gyroscope interrupt has been triggered
bool gyro_interrupt = false;

/**
 * Moving average filter used by gyroscope module
 */

// Filter depth
#define FILTER_SIZE 50

// Filter for each axis
static float filter[FILTER_SIZE][AXES_COUNT];

// Current index within filter for each axis
static int8_t filter_index[AXES_COUNT];

// Number of values in the filter which are currently occupied (not 0) for
// each axis
static int8_t filter_occupied[AXES_COUNT];

/**
 * Initializes the gyroscope.
 */
void gyro_init(void) {
    // Initialize gyroscope
    lsm9ds1_gyro_init_type init;
    init.data_rate = LSM9DS1_G_DATA_RATE_119;
    init.full_scale = LSM9DS1_G_FULL_SCALE_500;
    init.anti_aliasing_bw = LSM9DS1_G_ANTI_ALIASING_BW_MODE_1;
    init.axes_enable = LSM9DS1_G_AXIS_ENABLE_X | LSM9DS1_G_AXIS_ENABLE_Y |
        LSM9DS1_G_AXIS_ENABLE_Z;
    init.data_ready_interrupt_enabled = LSM9DS1_G_DR_INTERRUPT_ENABLED;
    lsm9ds1_gyro_init(&init);
    
    // Enable clock for 7-segment timer
    RCC_APB1PeriphClockCmd(GYRO_TIM_RCC, ENABLE);

    // Configure the 7-segment timer
    TIM_TimeBaseInitTypeDef tim_init;
    // Period and prescaler used to divide the clock frequency to the desired
    // timer frequency; see comments above for how these values were determined
    tim_init.TIM_Prescaler = GYRO_TIM_PERIOD - 1;
    tim_init.TIM_Period = GYRO_TIM_PRESCALER - 1;
    // Counter mode arbitrarily selected (either up or down could work)
    tim_init.TIM_CounterMode = TIM_CounterMode_Up;
    // No clock division (set divisior to /1) for digital filter (no external
    // triggering)
    tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(GYRO_TIM, &tim_init);

    // Enable the timer interrupt (from the timer point of view; we still have
    // to set up NVIC separately)
    TIM_ITConfig(GYRO_TIM, TIM_IT_Update, ENABLE);
    // Enable timer
    TIM_Cmd(GYRO_TIM, ENABLE);

    // Configure NVIC for timer so that timer ticks will trigger interrupts
    NVIC_InitTypeDef nvic_init;
    nvic_init.NVIC_IRQChannel = GYRO_NVIC_CHANNEL;
    nvic_init.NVIC_IRQChannelPreemptionPriority = GYRO_NVIC_PRIORITY;
    nvic_init.NVIC_IRQChannelSubPriority = GYRO_NVIC_SUBPRIORITY;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);
}

/**
 * Gets a calibrated gyroscope measurement using the calibration matrices
 * for a particular axis.
 *
 * @param filter_index The filter index.
 * @param axis_index   The axis index.
 *
 * @return The calibrated value.
 */
static float raw_to_calibrated(uint8_t filter_index, axes_type axis_index) {
    return calibration_matrix[axis_index][AXIS_X] *
            filter[filter_index][AXIS_X] +
           calibration_matrix[axis_index][AXIS_Y] *
            filter[filter_index][AXIS_Y] +
           calibration_matrix[axis_index][AXIS_Z] *
            filter[filter_index][AXIS_Z] +
           calibration_offset_matrix[axis_index];
}

/**
 * Gets a calibrated and filtered gyroscope reading using a moving average
 * filter for the specified axis index.
 *
 * @param axis_index The axis index.
 *
 * @return The calibrated and filtered value.
 */
static float raw_to_calibrated_filtered(axes_type axis_index) {
    // Take average of current value and (FILTER_SIZE - 1) previous values
    float running_sum = 0;
    int8_t count = 0;
    int8_t index = filter_index[axis_index];
    while (count < filter_occupied[axis_index]) {
        running_sum += raw_to_calibrated(index, axis_index);
        index -= 1;
        if (index < 0) {
            index = FILTER_SIZE - 1;
        }
        count += 1;
    }
    running_sum /= count;

    return running_sum;
}

/**
 * Gyroscope update function called by external line interrupt that gets
 * and stores the latest data from the gyroscope.
 */
void gyro_update(void) {
    lsm9ds1_get_angular_velocity(&filter[filter_index[AXIS_X]][AXIS_X],
                                 &filter[filter_index[AXIS_Y]][AXIS_Y],
                                 &filter[filter_index[AXIS_Z]][AXIS_Z]);
    for (uint8_t i = 0; i < AXES_COUNT; i++) {
        if (filter_occupied[i] < FILTER_SIZE) {
            filter_occupied[i] += 1;
        }
        filter_index[i] = (filter_index[i] + 1) % FILTER_SIZE;
    }

    gyro_interrupt = false;
}

/**
 * Interrupt handler for external interrupt line 1.
 */
void EXTI1_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line1);
        // Do nothing - replaced by timer
    }
}

/**
 * Interrupt handler for TIM3.
 */
void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(GYRO_TIM, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(GYRO_TIM, TIM_IT_Update);
        gyro_interrupt = true;
    }
}

/**
 * Gets the current angular velocity in the x direction. This value is filtered.
 *
 * @return The current angular velocity in the x direction.
 */
float gyro_get_x(void) {
    return raw_to_calibrated_filtered(AXIS_X);
}

/**
 * Gets the current angular velocity in the y direction. This value is filtered.
 *
 * @return The current angular velocity in the y direction.
 */
float gyro_get_y(void) {
    return raw_to_calibrated_filtered(AXIS_Y);
}

/**
 * Gets the current angular velocity in the z direction. This value is filtered.
 *
 * @return The current angular velocity in the z direction.
 */
float gyro_get_z(void) {
    return raw_to_calibrated_filtered(AXIS_Z);
}
