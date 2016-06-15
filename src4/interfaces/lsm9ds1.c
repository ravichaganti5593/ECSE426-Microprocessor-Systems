/**
  ******************************************************************************
  * @file    stm32f4_discovery_LSM9DS1.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    28-October-2011
  * @brief   This file provides a set of functions needed to manage the LSM9DS1
  *          MEMS accelerometer available on STM32F4-Discovery Kit.
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

/* Includes ------------------------------------------------------------------*/
#include "lsm9ds1.h"

/** @addtogroup Utilities
  * @{
  */ 

/** @addtogroup STM32F4_DISCOVERY
  * @{
  */ 

/** @addtogroup STM32F4_DISCOVERY_LSM9DS1
  * @{
  */


/** @defgroup STM32F4_DISCOVERY_LSM9DS1_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup STM32F4_DISCOVERY_LSM9DS1_Private_Defines
  * @{
  */
__IO uint32_t  LSM9DS1Timeout = LSM9DS1_FLAG_TIMEOUT;   

/* Read/Write command */
#define READWRITE_CMD              ((uint8_t)0x80) 
/* Multiple byte read/write command */ 
#define MULTIPLEBYTE_CMD           ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                 ((uint8_t)0x00)

/**
  * @}
  */

/** @defgroup STM32F4_DISCOVERY_LSM9DS1_Private_Macros
  * @{
  */

/**
  * @}
  */ 
  
/** @defgroup STM32F4_DISCOVERY_LSM9DS1_Private_Variables
  * @{
  */ 

/**
  * @}
  */

/** @defgroup STM32F4_DISCOVERY_LSM9DS1_Private_FunctionPrototypes
  * @{
  */
static uint8_t LSM9DS1_SendByte(uint8_t byte);
void LSM9DS1_LowLevel_Init(void);
/**
  * @}
  */

/** @defgroup STM32F4_DISCOVERY_LSM9DS1_Private_Functions
  * @{
  */


/**
  * @brief  Set LSM9DS1 Initialization.
  * @param  LSM9DS1_Config_Struct: pointer to a LSM9DS1_Config_TypeDef structure 
  *         that contains the configuration setting for the LSM9DS1.
  * @retval None
  */
void LSM9DS1_Init(LSM9DS1_InitTypeDef *LSM9DS1_InitStruct)
{
  uint8_t ctrl = 0x00;
  
  /* Configure the low level interface ---------------------------------------*/
  LSM9DS1_LowLevel_Init();
  
  /* Configure MEMS: data rate, full scale*/
  ctrl = (uint8_t) (LSM9DS1_InitStruct->Output_DataRate | LSM9DS1_InitStruct->Full_Scale | \
                    LSM9DS1_InitStruct->Bandwidth | LSM9DS1_InitStruct->AntiAliasingBandwidth);
  
  LSM9DS1_Write(&ctrl, LSM9DS1_CTRL_REG6_XL_ADDR, 1);
	
	ctrl = 0x00;
	ctrl = (uint8_t) (LSM9DS1_InitStruct->Axes_Enable);
	LSM9DS1_Write(&ctrl, LSM9DS1_CTRL_REG5_XL_ADDR, 1);
}

/**
  * @brief  Set LSM9DS1 Internal High Pass Filter configuration.
  * @param  LSM9DS1_Filter_ConfigTypeDef: pointer to a LSM9DS1_FilterConfig_TypeDef 
  *         structure that contains the configuration setting for the LSM9DS1 Filter.
  * @retval None
  */
void LSM9DS1_FilterConfig(LSM9DS1_FilterConfigTypeDef *LSM9DS1_FilterConfigStruct)
{
  uint8_t ctrl = 0x00;
  
  /* Read CTRL_REG2 register */
  LSM9DS1_Read(&ctrl, LSM9DS1_CTRL_REG2_ADDR, 1);
  
  /* Clear high pass filter cut-off level, interrupt and data selection bits*/
  ctrl &= (uint8_t)~(LSM9DS1_FILTEREDDATASELECTION_OUTPUTREGISTER | \
                     LSM9DS1_HIGHPASSFILTER_LEVEL_3 | \
                     LSM9DS1_HIGHPASSFILTERINTERRUPT_1_2);
  /* Configure MEMS high pass filter cut-off level, interrupt and data selection bits */                     
  ctrl |= (uint8_t)(LSM9DS1_FilterConfigStruct->HighPassFilter_Data_Selection | \
                    LSM9DS1_FilterConfigStruct->HighPassFilter_CutOff_Frequency | \
                    LSM9DS1_FilterConfigStruct->HighPassFilter_Interrupt);
  
  /* Write value to MEMS CTRL_REG2 register */
  LSM9DS1_Write(&ctrl, LSM9DS1_CTRL_REG2_ADDR, 1);
}

/**
  * @brief Set LSM9DS1 Interrupt configuration
  * @param  LSM9DS1_InterruptConfig_TypeDef: pointer to a LSM9DS1_InterruptConfig_TypeDef 
  *         structure that contains the configuration setting for the LSM9DS1 Interrupt.
  * @retval None
  */
void LSM9DS1_InterruptConfig(LSM9DS1_InterruptConfigTypeDef *LSM9DS1_IntConfigStruct)
{
  uint8_t ctrl = 0x00;
  
  /* Configure latch Interrupt request, click interrupts and double click interrupts */                   
  ctrl = (uint8_t)(LSM9DS1_IntConfigStruct->DataReadyInterrupt);
  
  /* Write value to MEMS CLICK_CFG register */
  LSM9DS1_Write(&ctrl, LSM9DS1_INT1_CTRL_REG_ADDR, 1);
}

/**
  * @brief  Change the lowpower mode for LSM9DS1
  * @param  LowPowerMode: new state for the lowpower mode.
  *   This parameter can be one of the following values:
  *     @arg LSM9DS1_LOWPOWERMODE_POWERDOWN: Power down mode
  *     @arg LSM9DS1_LOWPOWERMODE_ACTIVE: Active mode  
  * @retval None
  */
void LSM9DS1_LowpowerCmd(uint8_t LowPowerMode)
{
  uint8_t tmpreg;
  
  /* Read CTRL_REG1 register */
  LSM9DS1_Read(&tmpreg, LSM9DS1_CTRL_REG1_ADDR, 1);
  
  /* Set new low power mode configuration */
  tmpreg &= (uint8_t)~LSM9DS1_LOWPOWERMODE_ACTIVE;
  tmpreg |= LowPowerMode;
  
  /* Write value to MEMS CTRL_REG1 regsister */
  LSM9DS1_Write(&tmpreg, LSM9DS1_CTRL_REG1_ADDR, 1);
}

/**
  * @brief  Data Rate command 
  * @param  DataRateValue: Data rate value
  *   This parameter can be one of the following values:
  *     @arg LSM9DS1_DATARATE_100: 100 Hz output data rate 
  *     @arg LSM9DS1_DATARATE_400: 400 Hz output data rate    
  * @retval None
  */
void LSM9DS1_DataRateCmd(uint8_t DataRateValue)
{
  uint8_t tmpreg;
  
  /* Read CTRL_REG1 register */
  LSM9DS1_Read(&tmpreg, LSM9DS1_CTRL_REG1_ADDR, 1);
  
  /* Set new Data rate configuration */
  tmpreg &= (uint8_t)~LSM9DS1_DATARATE_400;
  tmpreg |= DataRateValue;
  
  /* Write value to MEMS CTRL_REG1 regsister */
  LSM9DS1_Write(&tmpreg, LSM9DS1_CTRL_REG1_ADDR, 1);
}

/**
  * @brief  Reboot memory content of LSM9DS1
  * @param  None
  * @retval None
  */
void LSM9DS1_RebootCmd(void)
{
  uint8_t tmpreg;
  /* Read CTRL_REG2 register */
  LSM9DS1_Read(&tmpreg, LSM9DS1_CTRL_REG2_ADDR, 1);
  
  /* Enable or Disable the reboot memory */
  tmpreg |= LSM9DS1_BOOT_REBOOTMEMORY;
  
  /* Write value to MEMS CTRL_REG2 regsister */
  LSM9DS1_Write(&tmpreg, LSM9DS1_CTRL_REG2_ADDR, 1);
}

/**
  * @brief  Writes one byte to the LSM9DS1.
  * @param  pBuffer : pointer to the buffer  containing the data to be written to the LSM9DS1.
  * @param  WriteAddr : LSM9DS1's internal address to write to.
  * @param  NumByteToWrite: Number of bytes to write.
  * @retval None
  */
void LSM9DS1_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  /* Configure the MS bit: 
       - When 0, the address will remain unchanged in multiple read/write commands.
       - When 1, the address will be auto incremented in multiple read/write commands.
  */
  if(NumByteToWrite > 0x01)
  {
    WriteAddr |= (uint8_t)MULTIPLEBYTE_CMD;
  }
  /* Set chip select Low at the start of the transmission */
  LSM9DS1_CS_LOW();
  
  /* Send the Address of the indexed register */
  LSM9DS1_SendByte(WriteAddr);
  /* Send the data that will be written into the device (MSB First) */
  while(NumByteToWrite >= 0x01)
  {
    LSM9DS1_SendByte(*pBuffer);
    NumByteToWrite--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  LSM9DS1_CS_HIGH();
}

/**
  * @brief  Reads a block of data from the LSM9DS1.
  * @param  pBuffer : pointer to the buffer that receives the data read from the LSM9DS1.
  * @param  ReadAddr : LSM9DS1's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the LSM9DS1.
  * @retval None
  */
void LSM9DS1_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
	if(NumByteToRead > 0x01)
  {
    ReadAddr |= (uint8_t)(READWRITE_CMD | MULTIPLEBYTE_CMD);
  }
  else
  {
    ReadAddr |= (uint8_t)READWRITE_CMD;
  }
  /* Set chip select Low at the start of the transmission */
  LSM9DS1_CS_LOW();
  
  /* Send the Address of the indexed register */
  LSM9DS1_SendByte(ReadAddr);
  
  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to LSM9DS1 (Slave device) */
    *pBuffer = LSM9DS1_SendByte(DUMMY_BYTE);
    NumByteToRead--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  LSM9DS1_CS_HIGH();
}

/**
  * @brief  Read LSM9DS1 output register, and calculate the acceleration 
  *         ACC[mg]=SENSITIVITY* (out_h*256+out_l)/16 (12 bit rappresentation)
  * @param  s16 buffer to store data
  * @retval None
  */
void LSM9DS1_ReadACC(int32_t* out)
{
  uint8_t buffer[6];
  uint8_t crtl, i = 0x00;
   
  LSM9DS1_Read(&crtl, LSM9DS1_CTRL_REG1_ADDR, 1);  
  LSM9DS1_Read(buffer, LSM9DS1_OUT_X_ADDR, 6);
  
  switch(crtl & 0x20) 
    {
    /* FS bit = 0 ==> Sensitivity typical value = 18milligals/digit*/ 
    case 0x00:
      for(i=0; i<0x03; i++)
      {
        *out =(int32_t)(LSM9DS1_SENSITIVITY_2_3G *  (int8_t)buffer[2*i]);
        out++;
      }
      break;
    /* FS bit = 1 ==> Sensitivity typical value = 72milligals/digit*/ 
    case 0x20:
      for(i=0; i<0x03; i++)
      {
        *out =(int32_t)(LSM9DS1_SENSITIVITY_9_2G * (int8_t)buffer[2*i]);
        out++;
      }         
      break;
    default:
      break;
    }
 }

/**
  * @brief  Initializes the low level interface used to drive the LSM9DS1
  * @param  None
  * @retval None
  */
void LSM9DS1_LowLevel_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  /* Enable the SPI periph */
  RCC_APB2PeriphClockCmd(LSM9DS1_SPI_CLK, ENABLE);

  /* Enable SCK, MOSI and MISO GPIO clocks */
  RCC_AHB1PeriphClockCmd(LSM9DS1_SPI_SCK_GPIO_CLK | LSM9DS1_SPI_MISO_GPIO_CLK | LSM9DS1_SPI_MOSI_GPIO_CLK, ENABLE);

  /* Enable CS  GPIO clock */
  RCC_AHB1PeriphClockCmd(LSM9DS1_SPI_CS_GPIO_CLK, ENABLE);
  
  /* Enable INT1 GPIO clock */
  RCC_AHB1PeriphClockCmd(LSM9DS1_SPI_INT1_GPIO_CLK, ENABLE);
  
  /* Enable INT2 GPIO clock */
  RCC_AHB1PeriphClockCmd(LSM9DS1_SPI_INT2_GPIO_CLK, ENABLE);

  GPIO_PinAFConfig(LSM9DS1_SPI_SCK_GPIO_PORT, LSM9DS1_SPI_SCK_SOURCE, LSM9DS1_SPI_SCK_AF);
  GPIO_PinAFConfig(LSM9DS1_SPI_MISO_GPIO_PORT, LSM9DS1_SPI_MISO_SOURCE, LSM9DS1_SPI_MISO_AF);
  GPIO_PinAFConfig(LSM9DS1_SPI_MOSI_GPIO_PORT, LSM9DS1_SPI_MOSI_SOURCE, LSM9DS1_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = LSM9DS1_SPI_SCK_PIN;
  GPIO_Init(LSM9DS1_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  LSM9DS1_SPI_MOSI_PIN;
  GPIO_Init(LSM9DS1_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin = LSM9DS1_SPI_MISO_PIN;
  GPIO_Init(LSM9DS1_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(LSM9DS1_SPI);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(LSM9DS1_SPI, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(LSM9DS1_SPI, ENABLE);

  /* Configure GPIO PIN for Lis Chip select */
  GPIO_InitStructure.GPIO_Pin = LSM9DS1_SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LSM9DS1_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

  /* Deselect : Chip Select high */
  GPIO_SetBits(LSM9DS1_SPI_CS_GPIO_PORT, LSM9DS1_SPI_CS_PIN);
  
  /* Configure GPIO PINs to detect Interrupts */
  GPIO_InitStructure.GPIO_Pin = LSM9DS1_SPI_INT1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(LSM9DS1_SPI_INT1_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = LSM9DS1_SPI_INT2_PIN;
  GPIO_Init(LSM9DS1_SPI_INT2_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitTypeDef gpio_init_s; // Structure to initilize definitions of GPIO
	GPIO_StructInit(&gpio_init_s); // Fills each GPIO_InitStruct member with its default value
	gpio_init_s.GPIO_Pin = GPIO_Pin_3; // Select the following pins to initialise
	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT; // Operating mode = output for the selected pins
	gpio_init_s.GPIO_Speed = GPIO_Speed_100MHz; // Don't limit slew rate, allow values to change as fast as they are set
	gpio_init_s.GPIO_OType = GPIO_OType_PP; // Operating output type (push-pull) for selected pins
	gpio_init_s.GPIO_PuPd = GPIO_PuPd_NOPULL; // If there is no input, don't pull.
	GPIO_Init(GPIOE, &gpio_init_s); // Initializes the GPIOD peripheral.
	
	GPIO_SetBits(GPIOE, GPIO_Pin_3);
}

/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received 
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval The received byte value
  */
static uint8_t LSM9DS1_SendByte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
  LSM9DS1Timeout = LSM9DS1_FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(LSM9DS1_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
    if((LSM9DS1Timeout--) == 0) return LSM9DS1_TIMEOUT_UserCallback();
  }
  
  /* Send a Byte through the SPI peripheral */
  SPI_I2S_SendData(LSM9DS1_SPI, byte);
  
  /* Wait to receive a Byte */
  LSM9DS1Timeout = LSM9DS1_FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(LSM9DS1_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
    if((LSM9DS1Timeout--) == 0) return LSM9DS1_TIMEOUT_UserCallback();
  }
  
  /* Return the Byte read from the SPI bus */
  return (uint8_t)SPI_I2S_ReceiveData(LSM9DS1_SPI);
}

//#ifdef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t LSM9DS1_TIMEOUT_UserCallback(void)
{
  /* Block communication and all processes */
  while (1)
  {   
  }
}
//#endif /* USE_DEFAULT_TIMEOUT_CALLBACK */

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
