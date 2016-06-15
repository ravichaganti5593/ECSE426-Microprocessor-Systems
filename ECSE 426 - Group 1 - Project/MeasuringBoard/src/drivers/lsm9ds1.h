#ifndef __LSM9DS1_H
#define __LSM9DS1_H

#include "stm32f4xx.h"

/**
 * Miscellaneous configuration
 */

#define LSM9DS1_FLAG_TIMEOUT                 ((uint32_t) 0x1000)

/**
 * Sensitivity configuration
 */

#define LSM9DS1_XL_SENSITIVITY_2             0.061f
#define LSM9DS1_XL_SENSITIVITY_4             0.122f
#define LSM9DS1_XL_SENSITIVITY_8             0.244f
#define LSM9DS1_XL_SENSITIVITY_16            0.732f

#define LSM9DS1_G_SENSITIVITY_245            8.75f
#define LSM9DS1_G_SENSITIVITY_500            17.50f
#define LSM9DS1_G_SENSITIVITY_2000           70.0f

/**
 * SPI, EXTI and GPIO configuration
 */

#define LSM9DS1_SPI                          SPI2
#define LSM9DS1_SPI_CLK                      RCC_APB1Periph_SPI2

#define LSM9DS1_SPI_SCK_PIN                  GPIO_Pin_13
#define LSM9DS1_SPI_SCK_GPIO_PORT            GPIOB
#define LSM9DS1_SPI_SCK_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define LSM9DS1_SPI_SCK_SOURCE               GPIO_PinSource13
#define LSM9DS1_SPI_SCK_AF                   GPIO_AF_SPI2

#define LSM9DS1_SPI_MISO_PIN                 GPIO_Pin_14
#define LSM9DS1_SPI_MISO_GPIO_PORT           GPIOB
#define LSM9DS1_SPI_MISO_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define LSM9DS1_SPI_MISO_SOURCE              GPIO_PinSource14
#define LSM9DS1_SPI_MISO_AF                  GPIO_AF_SPI2

#define LSM9DS1_SPI_MOSI_PIN                 GPIO_Pin_15
#define LSM9DS1_SPI_MOSI_GPIO_PORT           GPIOB
#define LSM9DS1_SPI_MOSI_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define LSM9DS1_SPI_MOSI_SOURCE              GPIO_PinSource15
#define LSM9DS1_SPI_MOSI_AF                  GPIO_AF_SPI2

#define LSM9DS1_SPI_CS_PIN                   GPIO_Pin_12
#define LSM9DS1_SPI_CS_GPIO_PORT             GPIOB
#define LSM9DS1_SPI_CS_GPIO_CLK              RCC_AHB1Periph_GPIOB

#define LSM9DS1_SPI_INT1_PIN                 GPIO_Pin_0
#define LSM9DS1_SPI_INT1_GPIO_PORT           GPIOE
#define LSM9DS1_SPI_INT1_GPIO_CLK            RCC_AHB1Periph_GPIOE
#define LSM9DS1_SPI_INT1_EXTI_LINE           EXTI_Line0
#define LSM9DS1_SPI_INT1_EXTI_PORT_SOURCE    EXTI_PortSourceGPIOE
#define LSM9DS1_SPI_INT1_EXTI_PIN_SOURCE     EXTI_PinSource0
#define LSM9DS1_SPI_INT1_EXTI_IRQn           EXTI0_IRQn

#define LSM9DS1_SPI_INT2_PIN                 GPIO_Pin_1
#define LSM9DS1_SPI_INT2_GPIO_PORT           GPIOE
#define LSM9DS1_SPI_INT2_GPIO_CLK            RCC_AHB1Periph_GPIOE
#define LSM9DS1_SPI_INT2_EXTI_LINE           EXTI_Line1
#define LSM9DS1_SPI_INT2_EXTI_PORT_SOURCE    EXTI_PortSourceGPIOE
#define LSM9DS1_SPI_INT2_EXTI_PIN_SOURCE     EXTI_PinSource1
#define LSM9DS1_SPI_INT2_EXTI_IRQn           EXTI1_IRQn

/**
 * Register mappings
 */

#define LSM9DS1_INT1_CTRL                    ((uint8_t) 0x0C)
#define LSM9DS1_INT2_CTRL                    ((uint8_t) 0x0D)
#define LSM9DS1_WHO_AM_I                     ((uint8_t) 0x0F)
#define LSM9DS1_CTRL_REG1_G                  ((uint8_t) 0x10)
#define LSM9DS1_OUT_X_L_G                    ((uint8_t) 0x18)
#define LSM9DS1_OUT_X_H_G                    ((uint8_t) 0x19)
#define LSM9DS1_OUT_Y_L_G                    ((uint8_t) 0x1A)
#define LSM9DS1_OUT_Y_H_G                    ((uint8_t) 0x1B)
#define LSM9DS1_OUT_Z_L_G                    ((uint8_t) 0x1C)
#define LSM9DS1_OUT_Z_H_G                    ((uint8_t) 0x1D)
#define LSM9DS1_CTRL_REG4                    ((uint8_t) 0x1E)
#define LSM9DS1_CTRL_REG5_XL                 ((uint8_t) 0x1F)
#define LSM9DS1_CTRL_REG6_XL                 ((uint8_t) 0x20)
#define LSM9DS1_CTRL_REG8                    ((uint8_t) 0x22)
#define LSM9DS1_OUT_X_L_XL                   ((uint8_t) 0x28)
#define LSM9DS1_OUT_X_H_XL                   ((uint8_t) 0x29)
#define LSM9DS1_OUT_Y_L_XL                   ((uint8_t) 0x2A)
#define LSM9DS1_OUT_Y_H_XL                   ((uint8_t) 0x2B)
#define LSM9DS1_OUT_Z_L_XL                   ((uint8_t) 0x2C)
#define LSM9DS1_OUT_Z_H_XL                   ((uint8_t) 0x2D)
#define LSM9DS1_FIFO_CTRL                    ((uint8_t) 0x2E)

/**
 * Options
 */

// LSM9DS1_INT1_CTRL

#define LSM9DS1_XL_DR_INTERRUPT_ENABLED      ((uint8_t) 0x01)
#define LSM9DS1_XL_DR_INTERRUPT_DISABLED     ((uint8_t) 0x00)

// LSM9DS1_INT2_CTRL

#define LSM9DS1_G_DR_INTERRUPT_ENABLED       ((uint8_t) 0x01 << 1)
#define LSM9DS1_G_DR_INTERRUPT_DISABLED      ((uint8_t) 0x00)

// LSM9DS1_CTRL_REG1_G

#define LSM9DS1_G_DATA_RATE_POWER_DOWN       ((uint8_t) 0x00 << 5)
#define LSM9DS1_G_DATA_RATE_14_9             ((uint8_t) 0x01 << 5)
#define LSM9DS1_G_DATA_RATE_59_5             ((uint8_t) 0x02 << 5)
#define LSM9DS1_G_DATA_RATE_119              ((uint8_t) 0x03 << 5)
#define LSM9DS1_G_DATA_RATE_238              ((uint8_t) 0x04 << 5)
#define LSM9DS1_G_DATA_RATE_476              ((uint8_t) 0x05 << 5)
#define LSM9DS1_G_DATA_RATE_952              ((uint8_t) 0x06 << 5)

#define LSM9DS1_G_FULL_SCALE_245             ((uint8_t) 0x00 << 3)
#define LSM9DS1_G_FULL_SCALE_500             ((uint8_t) 0x01 << 3)
#define LSM9DS1_G_FULL_SCALE_2000            ((uint8_t) 0x03 << 3)

#define LSM9DS1_G_ANTI_ALIASING_BW_MODE_1    ((uint8_t) 0x00)
#define LSM9DS1_G_ANTI_ALIASING_BW_MODE_2    ((uint8_t) 0x01)
#define LSM9DS1_G_ANTI_ALIASING_BW_MODE_3    ((uint8_t) 0x02)
#define LSM9DS1_G_ANTI_ALIASING_BW_MODE_4    ((uint8_t) 0x03)

// LSM9DS1_CTRL_REG4

#define LSM9DS1_G_AXIS_ENABLE_X              ((uint8_t) 0x01 << 3)
#define LSM9DS1_G_AXIS_ENABLE_Y              ((uint8_t) 0x01 << 4)
#define LSM9DS1_G_AXIS_ENABLE_Z              ((uint8_t) 0x01 << 5)

// LSM9DS1_CTRL_REG5_XL

#define LSM9DS1_XL_AXIS_ENABLE_X             ((uint8_t) 0x01 << 3)
#define LSM9DS1_XL_AXIS_ENABLE_Y             ((uint8_t) 0x01 << 4)
#define LSM9DS1_XL_AXIS_ENABLE_Z             ((uint8_t) 0x01 << 5)

// LSM9DS1_CTRL_REG6_XL

#define LSM9DS1_XL_DATA_RATE_POWER_DOWN      ((uint8_t) 0x00 << 5)
#define LSM9DS1_XL_DATA_RATE_10              ((uint8_t) 0x01 << 5)
#define LSM9DS1_XL_DATA_RATE_50              ((uint8_t) 0x02 << 5)
#define LSM9DS1_XL_DATA_RATE_119             ((uint8_t) 0x03 << 5)
#define LSM9DS1_XL_DATA_RATE_238             ((uint8_t) 0x04 << 5)
#define LSM9DS1_XL_DATA_RATE_476             ((uint8_t) 0x05 << 5)
#define LSM9DS1_XL_DATA_RATE_952             ((uint8_t) 0x06 << 5)

#define LSM9DS1_XL_FULL_SCALE_2              ((uint8_t) 0x00 << 3)
#define LSM9DS1_XL_FULL_SCALE_4              ((uint8_t) 0x02 << 3)
#define LSM9DS1_XL_FULL_SCALE_8              ((uint8_t) 0x03 << 3)
#define LSM9DS1_XL_FULL_SCALE_16             ((uint8_t) 0x01 << 3)

#define LSM9DS1_XL_ANTI_ALIASING_BW_AUTO     ((uint8_t) 0x00 << 2)
#define LSM9DS1_XL_ANTI_ALIASING_BW_MANUAL   ((uint8_t) 0x01 << 2)

#define LSM9DS1_XL_ANTI_ALIASING_BW_408      ((uint8_t) 0x00)
#define LSM9DS1_XL_ANTI_ALIASING_BW_211      ((uint8_t) 0x01)
#define LSM9DS1_XL_ANTI_ALIASING_BW_105      ((uint8_t) 0x02)
#define LSM9DS1_XL_ANTI_ALIASING_BW_50       ((uint8_t) 0x03)

// LSM9DS1_CTRL_REG8

#define LSM9DS1_BOOT_ON                      ((uint8_t) 0x01 << 7)
#define LSM9DS1_BOOT_OFF                     ((uint8_t) 0x00 << 7)

#define LSM9DS1_SW_RESET_ON                  ((uint8_t) 0x01)
#define LSM9DS1_SW_RESET_OFF                 ((uint8_t) 0x00)

/**
 * Initialization struct definitions
 */

typedef struct {
    uint8_t data_rate;
    uint8_t full_scale;
    uint8_t anti_aliasing_auto;
    uint8_t anti_aliasing_bw;
    uint8_t axes_enable;
    uint8_t data_ready_interrupt_enabled;
} lsm9ds1_acc_init_type;

typedef struct {
    uint8_t data_rate;
    uint8_t full_scale;
    uint8_t anti_aliasing_bw;
    uint8_t axes_enable;
    uint8_t data_ready_interrupt_enabled;
} lsm9ds1_gyro_init_type;

/**
 * Functions
 */

/**
* Performs a high-level initialization of the accelerometer.
*
* @param init The initialization struct.
*/
void lsm9ds1_acc_init(lsm9ds1_acc_init_type *init);

/**
* Performs a high-level initialization of the gyroscope.
*
* @param init The initialization struct.
*/
void lsm9ds1_gyro_init(lsm9ds1_gyro_init_type *init);

/**
 * Writes the specified bytes to the specified memory address within the
 * sensor via SPI.
 *
 * @param buf        The bytes to write.
 * @param write_addr The memory address within the sensor to write to.
 * @param num_bytes  The number of bytes to write.
 */
void lsm9ds1_write(uint8_t *buf, uint8_t write_addr, uint16_t num_bytes);

/**
 * Reads the specified number of bytes to the specified memory address within
 * the sensor via SPI.
 *
 * @param buf        The location to store the read bytes.
 * @param write_addr The memory address within the sensor to read from.
 * @param num_bytes  The number of bytes to read.
 */
void lsm9ds1_read(uint8_t *buf, uint8_t read_addr, uint16_t num_bytes);

/**
 * Gets the current acceleration for the x, y, and z axes from the accelerometer
 * via SPI.
 *
 * @param x Pointer to a buffer for the acceleration in the x direction.
 * @param y Pointer to a buffer for the acceleration in the y direction.
 * @param z Pointer to a buffer for the acceleration in the z direction.
 */
void lsm9ds1_get_acceleration(float *x, float *y, float *z);

/**
 * Gets the current angular velocity for the x, y, and z axes from the
 * gyroscope via SPI.
 *
 * @param x Pointer to a buffer for the angular velocity in the x direction.
 * @param y Pointer to a buffer for the angular velocity in the y direction.
 * @param z Pointer to a buffer for the angular velocity in the z direction.
 */
void lsm9ds1_get_angular_velocity(float *x, float *y, float *z);

#endif
