#include <stdbool.h>
#include <math.h>

#include "stm32f4xx.h"
#include "../drivers/lsm9ds1.h"

#include "acc.h"

/**
 * Accelerometer data and constants
 */

#define AXES_COUNT 3

typedef enum {
    AXIS_X,
    AXIS_Y,
    AXIS_Z
} axes_type;

#define ACC_TIM               TIM2
#define ACC_TIM_RCC           RCC_APB1Periph_TIM2

/**
 * The timer frequency is 100 Hz.
 *
 * The following formula was used to calculate the prescaler and period to get
 * this frequency:
 *
 * 100 Hz = 84000000 Hz / (prescaler * period)
 *
 * 5 and 56000 happens to be one integer solution to this formula, but others
 * would work.
 */
#define ACC_TIM_PRESCALER     (uint16_t) 15
#define ACC_TIM_PERIOD        (uint16_t) 56000

#define ACC_NVIC_CHANNEL      TIM2_IRQn
#define ACC_NVIC_PRIORITY     (uint8_t) 1
#define ACC_NVIC_SUBPRIORITY  (uint8_t) 0

// Calibration matrices used to calibrate the accelerometer readings. The
// matrices were derived from a 3 variable least-squares approximation on test
// data
static float calibration_matrix[AXES_COUNT][AXES_COUNT] = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
};
static float calibration_offset_matrix[AXES_COUNT] = {
    40,
    12,
    -6
};

// Flag indicating that the accelerometer interrupt has been triggered
bool acc_interrupt = false;

/**
 * Moving average filter used by accelerometer module
 */

// Filter depth
#define FILTER_SIZE 15

// Filter for each axis
static float filter[FILTER_SIZE][AXES_COUNT];

// Current index within filter for each axis
static int8_t filter_index[AXES_COUNT];

// Number of values in the filter which are currently occupied (not 0) for
// each axis
static int8_t filter_occupied[AXES_COUNT];

/**
 * Initializes the accelerometer.
 */
void acc_init(void) {
    // Initialize accelerometer
    lsm9ds1_acc_init_type init;
    init.data_rate = LSM9DS1_XL_DATA_RATE_119;
    init.full_scale = LSM9DS1_XL_FULL_SCALE_4;
    init.anti_aliasing_auto = LSM9DS1_XL_ANTI_ALIASING_BW_MANUAL;
    init.anti_aliasing_bw = LSM9DS1_XL_ANTI_ALIASING_BW_50;
    init.axes_enable = LSM9DS1_XL_AXIS_ENABLE_X | LSM9DS1_XL_AXIS_ENABLE_Y | 
        LSM9DS1_XL_AXIS_ENABLE_Z;
    init.data_ready_interrupt_enabled = LSM9DS1_XL_DR_INTERRUPT_ENABLED;
    lsm9ds1_acc_init(&init);
    
    // Enable clock for 7-segment timer
    RCC_APB1PeriphClockCmd(ACC_TIM_RCC, ENABLE);

    // Configure the 7-segment timer
    TIM_TimeBaseInitTypeDef tim_init;
    // Period and prescaler used to divide the clock frequency to the desired
    // timer frequency; see comments above for how these values were determined
    tim_init.TIM_Prescaler = ACC_TIM_PERIOD - 1;
    tim_init.TIM_Period = ACC_TIM_PRESCALER - 1;
    // Counter mode arbitrarily selected (either up or down could work)
    tim_init.TIM_CounterMode = TIM_CounterMode_Up;
    // No clock division (set divisior to /1) for digital filter (no external
    // triggering)
    tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(ACC_TIM, &tim_init);

    // Enable the timer interrupt (from the timer point of view; we still have
    // to set up NVIC separately)
    TIM_ITConfig(ACC_TIM, TIM_IT_Update, ENABLE);
    // Enable timer
    TIM_Cmd(ACC_TIM, ENABLE);

    // Configure NVIC for timer so that timer ticks will trigger interrupts
    NVIC_InitTypeDef nvic_init;
    nvic_init.NVIC_IRQChannel = ACC_NVIC_CHANNEL;
    nvic_init.NVIC_IRQChannelPreemptionPriority = ACC_NVIC_PRIORITY;
    nvic_init.NVIC_IRQChannelSubPriority = ACC_NVIC_SUBPRIORITY;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);
}


/**
 * Gets a calibrated accelerometer measurement using the calibration matrices
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
 * Gets a calibrated and filtered accelerometer reading using a moving average
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
 * Accelerometer update function called by external line interrupt that gets
 * and stores the latest data from the accelerometer.
 */
void acc_update(void) {
    lsm9ds1_get_acceleration(&filter[filter_index[AXIS_X]][AXIS_X],
                             &filter[filter_index[AXIS_Y]][AXIS_Y],
                             &filter[filter_index[AXIS_Z]][AXIS_Z]);
    
    for (uint8_t i = 0; i < AXES_COUNT; i++) {
        if (filter_occupied[i] < FILTER_SIZE) {
            filter_occupied[i] += 1;
        }
        filter_index[i] = (filter_index[i] + 1) % FILTER_SIZE;
    }

    acc_interrupt = false;
}

/**
 * Interrupt handler for external interrupt line 0.
 */
void EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line0);
        // Do nothing - replaced with timer
    }
}

/**
 * Interrupt handler for TIM2.
 */
void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(ACC_TIM, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(ACC_TIM, TIM_IT_Update);
        acc_interrupt = true;
    }
}

/**
 * Gets the current acceleration in the x direction. This value is filtered.
 *
 * @return The current acceleration in the x direction.
 */
float acc_get_x(void) {
    return raw_to_calibrated_filtered(AXIS_X);
}

/**
 * Gets the current acceleration in the y direction. This value is filtered.
 *
 * @return The current acceleration in the y direction.
 */
float acc_get_y(void) {
    return raw_to_calibrated_filtered(AXIS_Y);
}

/**
 * Gets the current acceleration in the z direction. This value is filtered.
 *
 * @return The current acceleration in the z direction.
 */
float acc_get_z(void) {
    return raw_to_calibrated_filtered(AXIS_Z);
}
