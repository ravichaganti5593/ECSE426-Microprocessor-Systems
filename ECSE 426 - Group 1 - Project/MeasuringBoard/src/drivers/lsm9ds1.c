#include "lsm9ds1.h"

/**
 * Driver constants and variables
 */

// Number of Cartesian axes (x, y, and z)
#define AXES_COUNT         ((uint8_t) 3)

// SPI r/w read command
#define SPI_RW_READ        ((uint8_t) 0x01 << 7)
// SPI r/w write command
#define SPI_RW_WRITE       ((uint8_t) 0x00 << 7)

// SPI burst (multiple byte read/write) on command
#define SPI_BURST_ON       ((uint8_t) 0x01 << 6)
// SPI burst (multiple byte read/write) on command
#define SPI_BURST_OFF      ((uint8_t) 0x00 << 6)

// SPI dummy byte
#define SPI_DUMMY_BYTE     ((uint8_t) 0x00)

// SPI timeout variable
__IO uint32_t lsm9ds1_timeout = LSM9DS1_FLAG_TIMEOUT;

/**
 * Functions
 */

/**
 * Selects the accelerometer on the chip select (CS) line.
 */
static inline void lsm9ds1_cs_low(void) {
    GPIO_ResetBits(LSM9DS1_SPI_CS_GPIO_PORT, LSM9DS1_SPI_CS_PIN);
}

/**
 * De-selects the accelerometer on the chip select (CS) line.
 */
static inline void lsm9ds1_cs_high(void) {
    GPIO_SetBits(LSM9DS1_SPI_CS_GPIO_PORT, LSM9DS1_SPI_CS_PIN);
}

/**
 * Performs a low-level initialization of the accelerometer.
 */
static void lsm9ds1_low_level_init(void) {
    // Enable SPI clock
    RCC_APB1PeriphClockCmd(LSM9DS1_SPI_CLK, ENABLE);

    // Enable SPI master-slave clock, MISO, MOSI, CS, INT1, and INT2 GPIO clocks
    RCC_AHB1PeriphClockCmd(LSM9DS1_SPI_SCK_GPIO_CLK  |
                           LSM9DS1_SPI_MISO_GPIO_CLK |
                           LSM9DS1_SPI_MOSI_GPIO_CLK |
                           LSM9DS1_SPI_CS_GPIO_CLK   |
                           LSM9DS1_SPI_INT1_GPIO_CLK |
                           LSM9DS1_SPI_INT2_GPIO_CLK, ENABLE);

    // Configure GPIO pin alternate functions for SPI master-slave clock, MISO,
    // and MOSI
    GPIO_PinAFConfig(LSM9DS1_SPI_SCK_GPIO_PORT, LSM9DS1_SPI_SCK_SOURCE,
                     LSM9DS1_SPI_SCK_AF);
    GPIO_PinAFConfig(LSM9DS1_SPI_MISO_GPIO_PORT, LSM9DS1_SPI_MISO_SOURCE,
                     LSM9DS1_SPI_MISO_AF);
    GPIO_PinAFConfig(LSM9DS1_SPI_MOSI_GPIO_PORT, LSM9DS1_SPI_MOSI_SOURCE,
                     LSM9DS1_SPI_MOSI_AF);

    // Default GPIO configuration for AF pins
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_DOWN;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;

    // Configure SPI master-slave clock GPIO pin as AF
    gpio_init.GPIO_Pin = LSM9DS1_SPI_SCK_PIN;
    GPIO_Init(LSM9DS1_SPI_SCK_GPIO_PORT, &gpio_init);

    // Configure SPI MOSI GPIO pin as AF
    gpio_init.GPIO_Pin = LSM9DS1_SPI_MOSI_PIN;
    GPIO_Init(LSM9DS1_SPI_MOSI_GPIO_PORT, &gpio_init);

    // Configure SPI MISO GPIO pin as AF
    gpio_init.GPIO_Pin = LSM9DS1_SPI_MISO_PIN;
    GPIO_Init(LSM9DS1_SPI_MISO_GPIO_PORT, &gpio_init);

    // Configure and enable SPI
    SPI_I2S_DeInit(LSM9DS1_SPI);
    SPI_InitTypeDef spi_init;
    spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi_init.SPI_DataSize = SPI_DataSize_8b;
    spi_init.SPI_CPOL = SPI_CPOL_Low;
    spi_init.SPI_CPHA = SPI_CPHA_1Edge;
    spi_init.SPI_NSS = SPI_NSS_Soft;
    spi_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
    spi_init.SPI_CRCPolynomial = 7;
    spi_init.SPI_Mode = SPI_Mode_Master;
    SPI_Init(LSM9DS1_SPI, &spi_init);
    SPI_Cmd(LSM9DS1_SPI, ENABLE);

    // Configure SPI CS GPIO pin as output
    gpio_init.GPIO_Pin = LSM9DS1_SPI_CS_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LSM9DS1_SPI_CS_GPIO_PORT, &gpio_init);
    // De-select accelerometer by default
    GPIO_SetBits(LSM9DS1_SPI_CS_GPIO_PORT, LSM9DS1_SPI_CS_PIN);

    // Configure accelerometer INT1 pin as input
    gpio_init.GPIO_Pin = LSM9DS1_SPI_INT1_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_IN;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(LSM9DS1_SPI_INT1_GPIO_PORT, &gpio_init);

    // Configure gyroscope INT2 pin as input
    gpio_init.GPIO_Pin = LSM9DS1_SPI_INT2_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_IN;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(LSM9DS1_SPI_INT2_GPIO_PORT, &gpio_init);
}

/**
 * Performs a low-level initialization of the interrupt component of the
 * accelerometer.
 */
static void lsm9ds1_acc_interrupt_low_level_init(void) {
    RCC_AHB1PeriphClockCmd(LSM9DS1_SPI_INT1_GPIO_CLK, ENABLE);
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Mode = GPIO_Mode_IN;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_DOWN;
    gpio_init.GPIO_Pin = LSM9DS1_SPI_INT1_PIN;
    GPIO_Init(LSM9DS1_SPI_INT1_GPIO_PORT, &gpio_init);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(LSM9DS1_SPI_INT1_EXTI_PORT_SOURCE,
                          LSM9DS1_SPI_INT1_EXTI_PIN_SOURCE);

    EXTI_InitTypeDef exti_init;
    exti_init.EXTI_Line = LSM9DS1_SPI_INT1_EXTI_LINE;
    exti_init.EXTI_LineCmd = ENABLE;
    exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&exti_init);

    NVIC_InitTypeDef nvic_init;
    nvic_init.NVIC_IRQChannel = LSM9DS1_SPI_INT1_EXTI_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_init.NVIC_IRQChannelSubPriority = 0;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);
}

/**
 * Performs a low-level initialization of the interrupt component of the
 * gyroscope.
 */
static void lsm9ds1_gyro_interrupt_low_level_init(void) {
    RCC_AHB1PeriphClockCmd(LSM9DS1_SPI_INT2_GPIO_CLK, ENABLE);
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Mode = GPIO_Mode_IN;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_DOWN;
    gpio_init.GPIO_Pin = LSM9DS1_SPI_INT2_PIN;
    GPIO_Init(LSM9DS1_SPI_INT2_GPIO_PORT, &gpio_init);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(LSM9DS1_SPI_INT2_EXTI_PORT_SOURCE,
                          LSM9DS1_SPI_INT2_EXTI_PIN_SOURCE);

    EXTI_InitTypeDef exti_init;
    exti_init.EXTI_Line = LSM9DS1_SPI_INT2_EXTI_LINE;
    exti_init.EXTI_LineCmd = ENABLE;
    exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&exti_init);

    NVIC_InitTypeDef nvic_init;
    nvic_init.NVIC_IRQChannel = LSM9DS1_SPI_INT2_EXTI_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_init.NVIC_IRQChannelSubPriority = 0;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);
}

/**
 * Performs a high-level initialization of the accelerometer.
 *
 * @param init The initialization struct.
 */
void lsm9ds1_acc_init(lsm9ds1_acc_init_type *init)
{
    // Perform low-level initialization
    lsm9ds1_low_level_init();

    // Write data rate, full scale, and anti-aliasing configuration to
    // LSM9DS1_CTRL_REG6_XL
    uint8_t buf = (uint8_t) (init->data_rate | init->full_scale |
                             init->anti_aliasing_auto | init->anti_aliasing_bw);
    lsm9ds1_write(&buf, LSM9DS1_CTRL_REG6_XL, 1);

    // Write axes enable to LSM9DS1_CTRL_REG5_XL_ADDR
	buf = (uint8_t) (init->axes_enable);
	lsm9ds1_write(&buf, LSM9DS1_CTRL_REG5_XL, 1);

    if (init->data_ready_interrupt_enabled) {
        lsm9ds1_acc_interrupt_low_level_init();

        // Write interrupt data ready to LSM9DS1_INT1_CTRL
    	buf = (uint8_t) (init->data_ready_interrupt_enabled);
    	lsm9ds1_write(&buf, LSM9DS1_INT1_CTRL, 1);
    }
    
    // Write bypass to LSM9DS1_FIFO_CTRL
    buf = (uint8_t) 0;
	lsm9ds1_write(&buf, LSM9DS1_FIFO_CTRL, 1);
}

/**
 * Performs a high-level initialization of the gyroscope.
 *
 * @param init The initialization struct.
 */
void lsm9ds1_gyro_init(lsm9ds1_gyro_init_type *init)
{
    // Perform low-level initialization
    lsm9ds1_low_level_init();

    // Write data rate, full scale, and anti-aliasing configuration to
    // LSM9DS1_CTRL_REG1_G
    uint8_t buf = (uint8_t) (init->data_rate | init->full_scale |
                             init->anti_aliasing_bw);
    lsm9ds1_write(&buf, LSM9DS1_CTRL_REG1_G, 1);

    // Write axes enable to LSM9DS1_CTRL_REG4
	buf = (uint8_t) (init->axes_enable);
	lsm9ds1_write(&buf, LSM9DS1_CTRL_REG4, 1);

    if (init->data_ready_interrupt_enabled) {
        lsm9ds1_gyro_interrupt_low_level_init();

        // Write interrupt data ready to LSM9DS1_INT2_CTRL
    	buf = (uint8_t) (init->data_ready_interrupt_enabled);
    	lsm9ds1_write(&buf, LSM9DS1_INT2_CTRL, 1);
    }
}

/**
 * Sends and receives a single byte over SPI.
 *
 * @param byte The byte to be sent.
 *
 * @return The byte received.
 */
static uint8_t lsm9ds1_send_recv_byte(uint8_t byte)
{
    // Wait until we are ready to send and receive a byte
    lsm9ds1_timeout = LSM9DS1_FLAG_TIMEOUT;
    while (SPI_I2S_GetFlagStatus(LSM9DS1_SPI, SPI_I2S_FLAG_TXE) == RESET) {
        // Use timeout to prevent SPI from blocking entire processor
        if (lsm9ds1_timeout-- == 0) {
            return 0;
        }
    }

    // Send a byte
    SPI_I2S_SendData(LSM9DS1_SPI, byte);

    // Wait to receive a byte
    lsm9ds1_timeout = LSM9DS1_FLAG_TIMEOUT;
    while (SPI_I2S_GetFlagStatus(LSM9DS1_SPI, SPI_I2S_FLAG_RXNE) == RESET) {
        // Use timeout to prevent SPI from blocking entire processor
        if (lsm9ds1_timeout-- == 0) {
            return 0;
        }
    }

    // Return the byte received
    return (uint8_t) SPI_I2S_ReceiveData(LSM9DS1_SPI);
}

/**
 * Writes the specified bytes to the specified memory address within the
 * accelerometer via SPI.
 *
 * @param buf        The bytes to write.
 * @param write_addr The memory address within the accelerometer to write to.
 * @param num_bytes  The number of bytes to write.
 */
void lsm9ds1_write(uint8_t *buf, uint8_t write_addr, uint16_t num_bytes)
{
    // Set SPI commands in command/address byte
    if (num_bytes > 1) {
        write_addr |= (uint8_t) (SPI_RW_WRITE | SPI_BURST_ON);
    } else {
        write_addr |= (uint8_t) (SPI_RW_WRITE | SPI_BURST_OFF);
    }

    // Prepare for data write by setting chip select active
    lsm9ds1_cs_low();

    // Send command byte and send bytes to be written to accelerometer
    lsm9ds1_send_recv_byte(write_addr);
    while (num_bytes >= 0x01)
    {
        lsm9ds1_send_recv_byte(*buf);
        num_bytes--;
        buf++;
    }

    // End data write by setting chip select inactive
    lsm9ds1_cs_high();
}

/**
 * Reads the specified number of bytes to the specified memory address within
 * the accelerometer via SPI.
 *
 * @param buf        The location to store the bytes that are read.
 * @param write_addr The memory address within the accelerometer to read from.
 * @param num_bytes  The number of bytes to read.
 */
void lsm9ds1_read(uint8_t *buf, uint8_t read_addr, uint16_t num_bytes)
{
    // Set SPI commands in command/address
    if (num_bytes > 1) {
        read_addr |= (uint8_t) (SPI_RW_READ | SPI_BURST_ON);
    } else {
        read_addr |= (uint8_t) (SPI_RW_READ | SPI_BURST_OFF);
    }

    // Prepare for data read by setting chip select active
    lsm9ds1_cs_low();

    // Send command byte and receive and store response bytes
    lsm9ds1_send_recv_byte(read_addr);
    while (num_bytes > 0x00)
    {
        *buf = lsm9ds1_send_recv_byte(SPI_DUMMY_BYTE);
        num_bytes--;
        buf++;
    }

    // End data read by setting chip select inactive
    lsm9ds1_cs_high();
}

/**
 * Gets the current acceleration for the x, y, and z axes from the accelerometer
 * via SPI.
 *
 * @param x Pointer to a buffer for the acceleration in the x direction.
 * @param y Pointer to a buffer for the acceleration in the y direction.
 * @param z Pointer to a buffer for the acceleration in the z direction.
 */
void lsm9ds1_get_acceleration(float *x, float *y, float *z) {
    // Get raw data from accelerometer and store in buffer
    uint8_t acc_raw[AXES_COUNT * 2];
    lsm9ds1_read(&acc_raw[0], LSM9DS1_OUT_X_L_XL, 1);
    lsm9ds1_read(&acc_raw[1], LSM9DS1_OUT_X_H_XL, 1);
    lsm9ds1_read(&acc_raw[2], LSM9DS1_OUT_Y_L_XL, 1);
    lsm9ds1_read(&acc_raw[3], LSM9DS1_OUT_Y_H_XL, 1);
    lsm9ds1_read(&acc_raw[4], LSM9DS1_OUT_Z_L_XL, 1);
    lsm9ds1_read(&acc_raw[5], LSM9DS1_OUT_Z_H_XL, 1);

    // Merge high and low registers into single value for x, y, and z axes
    int16_t acc[AXES_COUNT];
    for (uint8_t i = 0; i < AXES_COUNT; i++) {
        acc[i] = acc_raw[i * 2] + (int16_t) (acc_raw[i * 2 + 1] << 8);
    }

    // Determine sensitivity for value
    uint8_t full_scale_raw;
    lsm9ds1_read(&full_scale_raw, LSM9DS1_CTRL_REG6_XL, 1);
    uint8_t full_scale = (uint8_t) (full_scale_raw & (0x03 << 3));
    float sensitivity;
    if (full_scale == LSM9DS1_XL_FULL_SCALE_2) {
        sensitivity = LSM9DS1_XL_SENSITIVITY_2;
    } else if (full_scale == LSM9DS1_XL_FULL_SCALE_4) {
        sensitivity = LSM9DS1_XL_SENSITIVITY_4;
    } else if (full_scale == LSM9DS1_XL_FULL_SCALE_8) {
        sensitivity = LSM9DS1_XL_SENSITIVITY_8;
    } else if (full_scale == LSM9DS1_XL_FULL_SCALE_16) {
        sensitivity = LSM9DS1_XL_SENSITIVITY_16;
    }

    // Normalize each value and return acceleration for x, y, and z axes
    *x = acc[0] * sensitivity;
    *y = acc[1] * sensitivity;
    *z = acc[2] * sensitivity;
}

/**
 * Gets the current angular velocity for the x, y, and z axes from the
 * gyroscope via SPI.
 *
 * @param x Pointer to a buffer for the angular velocity in the x direction.
 * @param y Pointer to a buffer for the angular velocity in the y direction.
 * @param z Pointer to a buffer for the angular velocity in the z direction.
 */
void lsm9ds1_get_angular_velocity(float *x, float *y, float *z) {
    // Get raw data from gyroscope and store in buffer
    uint8_t gyro_raw[AXES_COUNT * 2];
    lsm9ds1_read(&gyro_raw[0], LSM9DS1_OUT_X_L_G, 1);
    lsm9ds1_read(&gyro_raw[1], LSM9DS1_OUT_X_H_G, 1);
    lsm9ds1_read(&gyro_raw[2], LSM9DS1_OUT_Y_L_G, 1);
    lsm9ds1_read(&gyro_raw[3], LSM9DS1_OUT_Y_H_G, 1);
    lsm9ds1_read(&gyro_raw[4], LSM9DS1_OUT_Z_L_G, 1);
    lsm9ds1_read(&gyro_raw[5], LSM9DS1_OUT_Z_H_G, 1);

    // Merge high and low registers into single value for x, y, and z axes
    int16_t gyro[AXES_COUNT];
    for (uint8_t i = 0; i < AXES_COUNT; i++) {
        gyro[i] = gyro_raw[i * 2] + (int16_t) (gyro_raw[i * 2 + 1] << 8);
    }

    // Determine sensitivity for value
    uint8_t full_scale_raw;
    lsm9ds1_read(&full_scale_raw, LSM9DS1_CTRL_REG1_G, 1);
    uint8_t full_scale = (uint8_t) (full_scale_raw & (0x03 << 3));
    float sensitivity;
    if (full_scale == LSM9DS1_G_FULL_SCALE_245) {
        sensitivity = LSM9DS1_G_SENSITIVITY_245;
    } else if (full_scale == LSM9DS1_G_FULL_SCALE_500) {
        sensitivity = LSM9DS1_G_SENSITIVITY_500;
    } else if (full_scale == LSM9DS1_G_FULL_SCALE_2000) {
        sensitivity = LSM9DS1_G_SENSITIVITY_2000;
    }

    // Normalize each value and return acceleration for x, y, and z axes
    *x = gyro[0] * sensitivity;
    *y = gyro[1] * sensitivity;
    *z = gyro[2] * sensitivity;
}
