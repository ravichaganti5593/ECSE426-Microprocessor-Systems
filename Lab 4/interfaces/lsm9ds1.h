/**
  ******************************************************************************
  * @file    stm32f4_discovery_LSM9DS1.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    28-October-2011
  * @brief   This file contains all the functions prototypes for the stm32f4_discovery_LSM9DS1.c
  *          firmware driver.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4_DISCOVERY_LSM9DS1_H
#define __STM32F4_DISCOVERY_LSM9DS1_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
 #include "stm32f4xx.h"
 #include "stm32f4xx_conf.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32F4_DISCOVERY
  * @{
  */ 

/** @addtogroup STM32F4_DISCOVERY_LSM9DS1
  * @{
  */
  

/** @defgroup STM32F4_DISCOVERY_LSM9DS1_Exported_Types
  * @{
  */
   
/* LSM9DS1 struct */
typedef struct
{
  uint8_t Power_Mode;                         /* Power-down/Active Mode */
  uint8_t Output_DataRate;                    /* OUT data rate 100 Hz / 400 Hz */
	uint8_t Bandwidth;
	uint8_t AntiAliasingBandwidth;
  uint8_t Axes_Enable;                        /* Axes enable */
  uint8_t Full_Scale;                         /* Full scale */
  uint8_t Self_Test;                          /* Self test */
}LSM9DS1_InitTypeDef;

/* LSM9DS1 High Pass Filter struct */
typedef struct
{
  uint8_t HighPassFilter_Data_Selection;      /* Internal filter bypassed or data from internal filter send to output register*/
  uint8_t HighPassFilter_CutOff_Frequency;    /* High pass filter cut-off frequency */
  uint8_t HighPassFilter_Interrupt;           /* High pass filter enabled for Freefall/WakeUp #1 or #2 */ 
}LSM9DS1_FilterConfigTypeDef;  

/* LSM9DS1 Interrupt struct */
typedef struct
{
  uint8_t DataReadyInterrupt;
}LSM9DS1_InterruptConfigTypeDef;  

/**
  * @}
  */
  
/** @defgroup STM32F4_DISCOVERY_LSM9DS1_Exported_Constants
  * @{
  */

/* Uncomment the following line to use the default LSM9DS1_TIMEOUT_UserCallback() 
   function implemented in stm32f4_discovery_LSM9DS1.c file.
   LSM9DS1_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occure during communication (waiting transmit data register empty flag(TXE)
   or waiting receive data register is not empty flag (RXNE)). */   
/* #define USE_DEFAULT_TIMEOUT_CALLBACK */

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define LSM9DS1_FLAG_TIMEOUT         ((uint32_t)0x1000)

/**
  * @brief  LSM9DS1 SPI Interface pins
  */
#define LSM9DS1_SPI                       SPI1
#define LSM9DS1_SPI_CLK                   RCC_APB2Periph_SPI1

#define LSM9DS1_SPI_SCK_PIN               GPIO_Pin_5                  /* PA.05 */
#define LSM9DS1_SPI_SCK_GPIO_PORT         GPIOA                       /* GPIOA */
#define LSM9DS1_SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define LSM9DS1_SPI_SCK_SOURCE            GPIO_PinSource5
#define LSM9DS1_SPI_SCK_AF                GPIO_AF_SPI1

#define LSM9DS1_SPI_MISO_PIN              GPIO_Pin_6                  /* PA.6 */
#define LSM9DS1_SPI_MISO_GPIO_PORT        GPIOA                       /* GPIOA */
#define LSM9DS1_SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define LSM9DS1_SPI_MISO_SOURCE           GPIO_PinSource6
#define LSM9DS1_SPI_MISO_AF               GPIO_AF_SPI1

#define LSM9DS1_SPI_MOSI_PIN              GPIO_Pin_7                  /* PA.7 */
#define LSM9DS1_SPI_MOSI_GPIO_PORT        GPIOA                       /* GPIOA */
#define LSM9DS1_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define LSM9DS1_SPI_MOSI_SOURCE           GPIO_PinSource7
#define LSM9DS1_SPI_MOSI_AF               GPIO_AF_SPI1

#define LSM9DS1_SPI_CS_PIN                GPIO_Pin_9                  /* PE.03 */
#define LSM9DS1_SPI_CS_GPIO_PORT          GPIOB                       /* GPIOE */
#define LSM9DS1_SPI_CS_GPIO_CLK           RCC_AHB1Periph_GPIOB

#define LSM9DS1_SPI_INT1_PIN              GPIO_Pin_0                  /* PE.00 */
#define LSM9DS1_SPI_INT1_GPIO_PORT        GPIOE                       /* GPIOE */
#define LSM9DS1_SPI_INT1_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define LSM9DS1_SPI_INT1_EXTI_LINE        EXTI_Line0
#define LSM9DS1_SPI_INT1_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define LSM9DS1_SPI_INT1_EXTI_PIN_SOURCE  EXTI_PinSource0
#define LSM9DS1_SPI_INT1_EXTI_IRQn        EXTI0_IRQn 

#define LSM9DS1_SPI_INT2_PIN              GPIO_Pin_1                  /* PE.01 */
#define LSM9DS1_SPI_INT2_GPIO_PORT        GPIOE                       /* GPIOE */
#define LSM9DS1_SPI_INT2_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define LSM9DS1_SPI_INT2_EXTI_LINE        EXTI_Line1
#define LSM9DS1_SPI_INT2_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define LSM9DS1_SPI_INT2_EXTI_PIN_SOURCE  EXTI_PinSource1
#define LSM9DS1_SPI_INT2_EXTI_IRQn        EXTI1_IRQn 


/******************************************************************************/
/*************************** START REGISTER MAPPING  **************************/
/******************************************************************************/

/*******************************************************************************
*  WHO_AM_I Register: Device Identification Register
*  Read only register
*  Default value: 0x3B
*******************************************************************************/
#define LSM9DS1_WHO_AM_I_ADDR                  0x0F

/*******************************************************************************
*  CTRL_REG1 Register: Control Register 1
*  Read Write register
*  Default value: 0x07
*  7 DR: Data Rate selection.
*        0 - 100 Hz output data rate
*        1 - 400 Hz output data rate
*  6 PD: Power Down control.
*        0 - power down mode
*        1 - active mode
*  5 FS: Full Scale selection.
*        0 - Typical measurement range 2.3
*        1 - Typical measurement range 9.2
*  4:3 STP-STM Self Test Enable:
*              STP |  STM |   mode
*            ----------------------------
*               0  |  0   |   Normal mode
*               0  |  1   |   Self Test M
*               1  |  0   |   Self Test P
*  2 Zen: Z axis enable.
*         0 - Z axis disabled
*         1- Z axis enabled
*  1 Yen: Y axis enable.
*         0 - Y axis disabled
*         1- Y axis enabled
*  0 Xen: X axis enable.
*         0 - X axis disabled
*         1- X axis enabled
********************************************************************************/
#define LSM9DS1_CTRL_REG1_ADDR                 0x10


#define LSM9DS1_CTRL_REG2_ADDR              0x11
#define LSM9DS1_CTRL_REG3_ADDR              0x12

//Enabling X, Y or Z
#define LSM9DS1_CTRL_REG5_XL_ADDR              0x1F

//Data rate, full scale, band width and anti aliasing
#define LSM9DS1_CTRL_REG6_XL_ADDR              0x20


#define LSM9DS1_HP_FILTER_RESET_REG_ADDR     0x23


#define LSM9DS1_STATUS_REG_ADDR             0x27

/*******************************************************************************
*  OUT_X Register: X-axis output Data
*  Read only register
*  Default value: output
*  7:0 XD7-XD0: X-axis output Data
*******************************************************************************/
#define LSM9DS1_OUT_X_ADDR                  0x28

/*******************************************************************************
*  OUT_Y Register: Y-axis output Data
*  Read only register
*  Default value: output
*  7:0 YD7-YD0: Y-axis output Data
*******************************************************************************/
#define LSM9DS1_OUT_Y_ADDR                  0x2A

/*******************************************************************************
*  OUT_Z Register: Z-axis output Data
*  Read only register
*  Default value: output
*  7:0 ZD7-ZD0: Z-axis output Data
*******************************************************************************/
#define LSM9DS1_OUT_Z_ADDR                  0x2C

#define LSM9DS1_FF_WU_CFG1_REG_ADDR         0x30

#define LSM9DS1_FF_WU_SRC1_REG_ADDR           0x31

#define LSM9DS1_FF_WU_THS1_REG_ADDR          0x32

#define LSM9DS1_FF_WU_DURATION1_REG_ADDR     0x33

#define LSM9DS1_FF_WU_CFG2_REG_ADDR         0x34

#define LSM9DS1_FF_WU_SRC2_REG_ADDR           0x35

/*******************************************************************************
*  FF_WU_THS_2 Register: Threshold register
*  Read Write register
*  Default value: 0x00
*  7 DCRM: Reset mode selection.
*          0 - counter resetted
*          1 - counter decremented
*  6 THS6-THS0: Free-fall/wake-up threshold value.
*******************************************************************************/
#define LSM9DS1_FF_WU_THS2_REG_ADDR          0x36

/*******************************************************************************
*  FF_WU_DURATION_2 Register: duration Register
*  Read Write register
*  Default value: 0x00
*  7:0 D7-D0 Duration value. (Duration steps and maximum values depend on the ODR chosen)
 ******************************************************************************/
#define LSM9DS1_FF_WU_DURATION2_REG_ADDR     0x37

/******************************************************************************
*  CLICK_CFG Register: click Register
*  Read Write register
*  Default value: 0x00
*  7 Reserved
*  6 LIR: Latch Interrupt request.
*         0: interrupt request not latched
*         1: interrupt request latched
*  5 Double_Z: Enable interrupt generation on double click event on Z axis.
*              0: disable interrupt request
*              1: enable interrupt request
*  4 Single_Z: Enable interrupt generation on single click event on Z axis.
*              0: disable interrupt request
*              1: enable interrupt request
*  3 Double_Y: Enable interrupt generation on double click event on Y axis.
*              0: disable interrupt request
*              1: enable interrupt request
*  2 Single_Y: Enable interrupt generation on single click event on Y axis.
*              0: disable interrupt request
*              1: enable interrupt request
*  1 Double_X: Enable interrupt generation on double click event on X axis.
*              0: disable interrupt request
*              1: enable interrupt request
*  0 Single_y: Enable interrupt generation on single click event on X axis.
*              0: disable interrupt request
*              1: enable interrupt request
 ******************************************************************************/
#define LSM9DS1_CLICK_CFG_REG_ADDR     0x38

//Determine various interrupts (most interested in data ready interrupt)
#define LSM9DS1_INT1_CTRL_REG_ADDR     0x0C

/******************************************************************************
*  CLICK_SRC Register: click status Register
*  Read only register
*  Default value: 0x00
*  7 Reserved
*  6 IA: Interrupt active.
*        0: no interrupt has been generated
*        1: one or more interrupts have been generated
*  5 Double_Z: Double click on Z axis event.
*        0: no interrupt
*        1: Double Z event has occurred 
*  4 Single_Z: Z low.
*        0: no interrupt
*        1: Single Z event has occurred 
*  3 Double_Y: Y high.
*        0: no interrupt
*        1: Double Y event has occurred 
*  2 Single_Y: Y low.
*        0: no interrupt
*        1: Single Y event has occurred 
*  1 Double_X: X high.
*        0: no interrupt
*        1: Double X event has occurred 
*  0 Single_X: X low.
*        0: no interrupt
*        1: Single X event has occurred 
*******************************************************************************/
#define LSM9DS1_CLICK_SRC_REG_ADDR        0x39

/*******************************************************************************
*  CLICK_THSY_X Register: Click threshold Y and X register
*  Read Write register
*  Default value: 0x00
*  7:4 THSy3-THSy0: Click threshold on Y axis, step 0.5g
*  3:0 THSx3-THSx0: Click threshold on X axis, step 0.5g
*******************************************************************************/
#define LSM9DS1_CLICK_THSY_X_REG_ADDR        0x3B

/*******************************************************************************
*  CLICK_THSZ Register: Click threshold Z register
*  Read Write register
*  Default value: 0x00
*  7:4 Reserved
*  3:0 THSz3-THSz0: Click threshold on Z axis, step 0.5g
*******************************************************************************/
#define LSM9DS1_CLICK_THSZ_REG_ADDR         0x3C

/*******************************************************************************
*  CLICK_TimeLimit Register: Time Limit register
*  Read Write register
*  Default value: 0x00
*  7:0 Dur7-Dur0: Time Limit value, step 0.5g
*******************************************************************************/
#define LSM9DS1_CLICK_TIMELIMIT_REG_ADDR        0x3D

/*******************************************************************************
*  CLICK_Latency Register: Latency register
*  Read Write register
*  Default value: 0x00
*  7:0 Lat7-Lat0: Latency value, step 1msec
*******************************************************************************/
#define LSM9DS1_CLICK_LATENCY_REG_ADDR        0x3E

/*******************************************************************************
*  CLICK_Window Register: Window register
*  Read Write register
*  Default value: 0x00
*  7:0 Win7-Win0: Window value, step 1msec
*******************************************************************************/
#define LSM9DS1_CLICK_WINDOW_REG_ADDR        0x3F

/******************************************************************************/
/**************************** END REGISTER MAPPING  ***************************/
/******************************************************************************/

#define LSM9DS1_SENSITIVITY_2_3G                         18  /* 18 mg/digit*/
#define LSM9DS1_SENSITIVITY_9_2G                         72  /* 72 mg/digit*/

/** @defgroup Data_Rate_selection                 
  * @{
  */
#define LSM9DS1_DATARATE_100                             ((uint8_t)(3 << 5))
#define LSM9DS1_DATARATE_400                             ((uint8_t)(5 << 5))
/**
  * @}
  */
  
/** @defgroup Power_Mode_selection 
  * @{
  */
#define LSM9DS1_LOWPOWERMODE_POWERDOWN                   ((uint8_t)0x00)
#define LSM9DS1_LOWPOWERMODE_ACTIVE                      ((uint8_t)0x40)
/**
  * @}
  */
  
/** @defgroup Full_Scale_selection 
  * @{
  */
#define LSM9DS1_FULLSCALE_2G                             ((uint8_t)(0 << 3))
#define LSM9DS1_FULLSCALE_4G                             ((uint8_t)(2 << 3))
#define LSM9DS1_FULLSCALE_8G                             ((uint8_t)(3 << 3))
#define LSM9DS1_FULLSCALE_16G                            ((uint8_t)(1 << 3))

/**
  * @}
  */
	
/** @defgroup Bandwidth_selection 
  * @{
  */
#define LSM9DS1_BANDWIDTH_ODR_BASED                             ((uint8_t)(0 << 2))
#define LSM9DS1_BANDWIDTH_ANTI_ALIASING_BASED                   ((uint8_t)(1 << 2))

/**
  * @}
  */
	
/**
  * @}
  */
	
/** @defgroup AntiAliasing_Bandwidth_selection 
  * @{
  */
#define LSM9DS1_ANTI_ALIASING_BANDWIDTH_408_HZ                  ((uint8_t)(0))
#define LSM9DS1_ANTI_ALIASING_BANDWIDTH_211_HZ                  ((uint8_t)(1))
#define LSM9DS1_ANTI_ALIASING_BANDWIDTH_105_HZ                  ((uint8_t)(2))
#define LSM9DS1_ANTI_ALIASING_BANDWIDTH_050_HZ                  ((uint8_t)(3))

/**
  * @}
  */
  
/** @defgroup Self_Test_selection 
  * @{
  */
#define LSM9DS1_SELFTEST_NORMAL                          ((uint8_t)0x00)
#define LSM9DS1_SELFTEST_P                               ((uint8_t)0x10)
#define LSM9DS1_SELFTEST_M                               ((uint8_t)0x08)
/**
  * @}
  */  

/** @defgroup Direction_XYZ_selection 
  * @{
  */
#define LSM9DS1_X_ENABLE                                 ((uint8_t)(1 << 3))
#define LSM9DS1_Y_ENABLE                                 ((uint8_t)(1 << 4))
#define LSM9DS1_Z_ENABLE                                 ((uint8_t)(1 << 5))
#define LSM9DS1_XYZ_ENABLE                               ((uint8_t)(7 << 3))
/**
  * @}
  */
 
 /** @defgroup SPI_Serial_Interface_Mode_selection 
  * @{
  */
#define LSM9DS1_SERIALINTERFACE_4WIRE                    ((uint8_t)0x00)
#define LSM9DS1_SERIALINTERFACE_3WIRE                    ((uint8_t)0x80)
/**
  * @}
  */ 

 /** @defgroup Boot_Mode_selection 
  * @{
  */
#define LSM9DS1_BOOT_NORMALMODE                          ((uint8_t)0x00)
#define LSM9DS1_BOOT_REBOOTMEMORY                        ((uint8_t)0x40)
/**
  * @}
  */   

 /** @defgroup Filtered_Data_Selection_Mode_selection 
  * @{
  */
#define LSM9DS1_FILTEREDDATASELECTION_BYPASSED           ((uint8_t)0x00)
#define LSM9DS1_FILTEREDDATASELECTION_OUTPUTREGISTER     ((uint8_t)0x20)
/**
  * @}
  */ 
  
 /** @defgroup High_Pass_Filter_Interrupt_selection 
  * @{
  */  
#define LSM9DS1_HIGHPASSFILTERINTERRUPT_OFF              ((uint8_t)0x00)
#define LSM9DS1_HIGHPASSFILTERINTERRUPT_1                ((uint8_t)0x04)
#define LSM9DS1_HIGHPASSFILTERINTERRUPT_2                ((uint8_t)0x08)
#define LSM9DS1_HIGHPASSFILTERINTERRUPT_1_2              ((uint8_t)0x0C)
/**
  * @}
  */ 
  
 /** @defgroup High_Pass_Filter_selection 
  * @{
  */
#define LSM9DS1_HIGHPASSFILTER_LEVEL_0                   ((uint8_t)0x00)
#define LSM9DS1_HIGHPASSFILTER_LEVEL_1                   ((uint8_t)0x01)
#define LSM9DS1_HIGHPASSFILTER_LEVEL_2                   ((uint8_t)0x02)
#define LSM9DS1_HIGHPASSFILTER_LEVEL_3                   ((uint8_t)0x03)
/**
  * @}
  */

/** @defgroup Data_Ready_Interrupt_selection 
  * @{
  */
#define LSM9DS1_INTERRUPT_DATA_READY_OFF                ((uint8_t)0x00)
#define LSM9DS1_INTERRUPT_DATA_READY_ON                 ((uint8_t)0x01)
/**
  * @}
  */

/** @defgroup latch_Interrupt_Request_selection 
  * @{
  */
#define LSM9DS1_INTERRUPTREQUEST_NOTLATCHED              ((uint8_t)0x00)
#define LSM9DS1_INTERRUPTREQUEST_LATCHED                 ((uint8_t)0x40)
/**
  * @}
  */

/** @defgroup Click_Interrupt_XYZ_selection 
  * @{
  */
#define LSM9DS1_CLICKINTERRUPT_XYZ_DISABLE               ((uint8_t)0x00)
#define LSM9DS1_CLICKINTERRUPT_X_ENABLE                  ((uint8_t)0x01)
#define LSM9DS1_CLICKINTERRUPT_Y_ENABLE                  ((uint8_t)0x04)
#define LSM9DS1_CLICKINTERRUPT_Z_ENABLE                  ((uint8_t)0x10)
#define LSM9DS1_CLICKINTERRUPT_XYZ_ENABLE                ((uint8_t)0x15)
/**
  * @}
  */

/** @defgroup Double_Click_Interrupt_XYZ_selection 
  * @{
  */
#define LSM9DS1_DOUBLECLICKINTERRUPT_XYZ_DISABLE         ((uint8_t)0x00)
#define LSM9DS1_DOUBLECLICKINTERRUPT_X_ENABLE            ((uint8_t)0x02)
#define LSM9DS1_DOUBLECLICKINTERRUPT_Y_ENABLE            ((uint8_t)0x08)
#define LSM9DS1_DOUBLECLICKINTERRUPT_Z_ENABLE            ((uint8_t)0x20)
#define LSM9DS1_DOUBLECLICKINTERRUPT_XYZ_ENABLE          ((uint8_t)0x2A)
/**
  * @}
  */
/**
  * @}
  */ 
  
/** @defgroup STM32F4_DISCOVERY_LSM9DS1_Exported_Macros
  * @{
  */
#define LSM9DS1_CS_LOW()       GPIO_ResetBits(LSM9DS1_SPI_CS_GPIO_PORT, LSM9DS1_SPI_CS_PIN)
#define LSM9DS1_CS_HIGH()      GPIO_SetBits(LSM9DS1_SPI_CS_GPIO_PORT, LSM9DS1_SPI_CS_PIN)
/**
  * @}
  */ 

/** @defgroup STM32F4_DISCOVERY_LSM9DS1_Exported_Functions
  * @{
  */ 
void LSM9DS1_LowLevel_Init(void);
void LSM9DS1_Init(LSM9DS1_InitTypeDef *LSM9DS1_InitStruct);
void LSM9DS1_InterruptConfig(LSM9DS1_InterruptConfigTypeDef *LSM9DS1_InterruptConfigStruct);
void LSM9DS1_FilterConfig(LSM9DS1_FilterConfigTypeDef *LSM9DS1_FilterConfigStruct);
void LSM9DS1_LowpowerCmd(uint8_t LowPowerMode);
void LSM9DS1_FullScaleCmd(uint8_t FS_value);
void LSM9DS1_DataRateCmd(uint8_t DataRateValue);
void LSM9DS1_RebootCmd(void);
void LSM9DS1_ReadACC(int32_t* out);
void LSM9DS1_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
void LSM9DS1_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);

/* USER Callbacks: This is function for which prototype only is declared in
   MEMS accelerometre driver and that should be implemented into user applicaiton. */  
/* LSM9DS1_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occure during communication (waiting transmit data register empty flag(TXE)
   or waiting receive data register is not empty flag (RXNE)).
   You can use the default timeout callback implementation by uncommenting the 
   define USE_DEFAULT_TIMEOUT_CALLBACK in stm32f4_discovery_LSM9DS1.h file.
   Typically the user implementation of this callback should reset MEMS peripheral
   and re-initialize communication or in worst case reset all the application. */
uint32_t LSM9DS1_TIMEOUT_UserCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4_DISCOVERY_LSM9DS1_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
