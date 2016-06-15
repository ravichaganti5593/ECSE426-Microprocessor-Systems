#include "cc2500.h"
#include "interfaces/cc2500_settings.h"


__IO uint32_t  CC2500Timeout = CC2500_FLAG_TIMEOUT;   

/* Read/Write command */
#define READWRITE_CMD              ((uint8_t)0x80) 
/* Multiple byte read/write command */ 
#define MULTIPLEBYTE_CMD           ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                 ((uint8_t)0x00)


/////////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/*************************** START REGISTER MAPPING  **************************/
/******************************************************************************/
static const uint8_t CC2500_SRES = 0x30;
static const uint8_t CC2500_SNOP = 0x3D;
static const uint8_t CC2500_SFRX = 0x3A;
static const uint8_t CC2500_SFTX = 0x3B;

static const uint8_t CC2500_PARTNUM = 0x30 | 0xC0;
static const uint8_t CC2500_MARCSTATE = 0x35 | 0xC0;
static const uint8_t CC2500_TXBYTES = 0x3A | 0xC0;
static const uint8_t CC2500_RXBYTES = 0x3B | 0xC0;


/////////////////////////////////////////////////////////////////////////////////////////////////


static uint8_t CC2500_SendByte(uint8_t byte);
void CC2500_LowLevel_Init(void);


/**
	* @brief  Writes one byte to the CC2500.
	* @param  pBuffer : pointer to the buffer  containing the data to be written to the CC2500.
	* @param  WriteAddr : CC2500's internal address to write to.
	* @param  NumByteToWrite: Number of bytes to write.
	* @retval None
	*/
void CC2500_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
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
	CC2500_CS_LOW();
	
	/* Send the Address of the indexed register */
	CC2500_SendByte(WriteAddr);
	/* Send the data that will be written into the device (MSB First) */
	while(NumByteToWrite >= 0x01)
	{
		CC2500_SendByte(*pBuffer);
		NumByteToWrite--;
		pBuffer++;
	}
	
	/* Set chip select High at the end of the transmission */ 
	CC2500_CS_HIGH();
}

/**
	* @brief  Reads a block of data from the CC2500.
	* @param  pBuffer : pointer to the buffer that receives the data read from the CC2500.
	* @param  ReadAddr : CC2500's internal address to read from.
	* @param  NumByteToRead : number of bytes to read from the CC2500.
	* @retval None
	*/
void CC2500_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
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
	CC2500_CS_LOW();
	
	/* Send the Address of the indexed register */
	CC2500_SendByte(ReadAddr);
	
	/* Receive the data that will be read from the device (MSB First) */
	while(NumByteToRead > 0x00)
	{
		/* Send dummy byte (0x00) to generate the SPI clock to CC2500 (Slave device) */
		*pBuffer = CC2500_SendByte(DUMMY_BYTE);
		NumByteToRead--;
		pBuffer++;
	}
	
	/* Set chip select High at the end of the transmission */ 
	CC2500_CS_HIGH();
}

/**
 * @brief  Read one byte from CC2500.
 * @param  ReadAddr : address to read from
 * @retval byte value read from CC2500
 */
__inline uint8_t CC2500_read_one(uint8_t ReadAddr) {
	uint8_t temp;
	CC2500_Read(&temp, ReadAddr, 1);
	return temp;
}

/**
 * @brief  Write one byte to CC2500.
 * @param  WriteAddr : address to write to
 * @retval void
 */
__inline void CC2500_write_one(uint8_t* value, uint8_t WriteAddr) {
	CC2500_Write(value, WriteAddr, 1);
}

/**
 * @brief  Get SFM state of CC2500.
 * @param void
 * @retval SFM state of CC2500
 */
__inline uint8_t CC2500_get_state(void) {
	return CC2500_read_one(CC2500_MARCSTATE);
}

/**
 * @brief  Get part num of CC2500.
 * @param void
 * @retval part num of CC2500
 */
__inline uint8_t CC2500_get_part_num(void) {
	return CC2500_read_one(CC2500_PARTNUM);
}

/**
 * @brief  Get number of bytes in RX FIFO of CC2500.
 * @param void
 * @retval number of bytes in RX FIFO of CC2500
 */
__inline uint8_t CC2500_get_rxbytes(void) {
	return CC2500_read_one(CC2500_RXBYTES);
}

/**
 * @brief  Get number of bytes in TX FIFO of CC2500.
 * @param void
 * @retval number of bytes in TX FIFO of CC2500
 */
__inline uint8_t CC2500_get_txbytes(void) {
	return CC2500_read_one(CC2500_TXBYTES);
}

/**
 * @brief  Flush RX FIFO of CC2500.
 * @param void
 * @retval byte received from CC2500
 */
__inline uint8_t CC2500_flush_rx(void) {
	return CC2500_read_one(CC2500_SFRX);
}

/**
 * @brief  Flush TX FIFO of CC2500.
 * @param void
 * @retval byte received from CC2500
 */
__inline uint8_t CC2500_flush_tx(void) {
	return CC2500_read_one(CC2500_SFTX);
}

/**
 * @brief  Read one byte from RX FIFO of CC2500.
 * @param void
 * @retval byte received from CC2500
 */
__inline uint8_t CC2500_read_rx_one(void) {
	return CC2500_read_one(CC2500_RX_FIFO);
}

/**
 * @brief  Read multiple bytes from RX FIFO of CC2500.
 * @param buffer buffer to write result to
 * @param NumByteToRead number of bytes to read from RX FIFO
 * @retval void
 */
__inline void CC2500_read_rx(uint8_t* buffer, uint8_t NumByteToRead) {
	if (NumByteToRead == 1) {
		CC2500_Read(buffer, CC2500_RX_FIFO, NumByteToRead);
	} else if (NumByteToRead > 1) {
		CC2500_Read(buffer, CC2500_RX_FIFO_BURST, NumByteToRead);
	}
}

/**
 * @brief  Write one byte to TX FIFO of CC2500.
 * @param value value to write
 * @retval void
 */
__inline void CC2500_write_tx_one(uint8_t value) {
	CC2500_write_one(&value, CC2500_TX_FIFO);
}

/**
 * @brief  Write multiple bytes to TX FIFO of CC2500.
 * @param buffer buffer to read values from
 * @param NumByteToWrite number of bytes to write to TX FIFO
 * @retval void
 */
__inline void CC2500_write_tx(uint8_t* buffer, uint8_t NumByteToWrite) {
	if (NumByteToWrite == 1) {
		CC2500_Write(buffer, CC2500_TX_FIFO, NumByteToWrite);
	} else if (NumByteToWrite > 1) {
		CC2500_Write(buffer, CC2500_TX_FIFO_BURST, NumByteToWrite);	
	}
}

/**
 * @brief  Reset CC2500 chip.
 * @param  void
 * @retval None
 */
void CC2500_Reset(void) {
	static uint8_t value;
	value = CC2500_read_one(CC2500_SRES);
	do {
		value = CC2500_get_state();
	} while (value != 1);

	for (uint8_t i = 0; i < CC2500_CONFIG_COUNT; i++) {
		CC2500_write_one(&(cc2500_config[i][0]), cc2500_config[i][1]);
	}

	CC2500_flush_rx();
	CC2500_flush_tx();
}

/**
	* @brief  Initializes the low level interface used to drive the CC2500
	* @param  None
	* @retval None
	*/
void CC2500_LowLevel_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	/* Enable the SPI periph */
	RCC_APB2PeriphClockCmd(CC2500_SPI_CLK, ENABLE);

	/* Enable SCK, MOSI and MISO GPIO clocks */
	RCC_AHB1PeriphClockCmd(CC2500_SPI_SCK_GPIO_CLK | CC2500_SPI_MISO_GPIO_CLK | CC2500_SPI_MOSI_GPIO_CLK, ENABLE);

	/* Enable CS  GPIO clock */
	RCC_AHB1PeriphClockCmd(CC2500_SPI_CS_GPIO_CLK, ENABLE);
	
	GPIO_PinAFConfig(CC2500_SPI_SCK_GPIO_PORT, CC2500_SPI_SCK_SOURCE, CC2500_SPI_SCK_AF);
	GPIO_PinAFConfig(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_SOURCE, CC2500_SPI_MISO_AF);
	GPIO_PinAFConfig(CC2500_SPI_MOSI_GPIO_PORT, CC2500_SPI_MOSI_SOURCE, CC2500_SPI_MOSI_AF);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = CC2500_SPI_SCK_PIN;
	GPIO_Init(CC2500_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	/* SPI  MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  CC2500_SPI_MOSI_PIN;
	GPIO_Init(CC2500_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/* SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin = CC2500_SPI_MISO_PIN;
	GPIO_Init(CC2500_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(CC2500_SPI);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(CC2500_SPI, &SPI_InitStructure);

	/* Enable SPI1  */
	SPI_Cmd(CC2500_SPI, ENABLE);

	/* Configure GPIO PIN for Lis Chip select */
	GPIO_InitStructure.GPIO_Pin = CC2500_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CC2500_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

	/* Deselect : Chip Select high */
	GPIO_SetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN);
}

/**
	* @brief  Sends a Byte through the SPI interface and return the Byte received 
	*         from the SPI bus.
	* @param  Byte : Byte send.
	* @retval The received byte value
	*/
static uint8_t CC2500_SendByte(uint8_t byte)
{
	/* Loop while DR register in not emplty */
	CC2500Timeout = CC2500_FLAG_TIMEOUT;
	while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE) == RESET)
	{
		if((CC2500Timeout--) == 0) return CC2500_TIMEOUT_UserCallback();
	}
	
	/* Send a Byte through the SPI peripheral */
	SPI_I2S_SendData(CC2500_SPI, byte);
	
	/* Wait to receive a Byte */
	CC2500Timeout = CC2500_FLAG_TIMEOUT;
	while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((CC2500Timeout--) == 0) return CC2500_TIMEOUT_UserCallback();
	}
	
	/* Return the Byte read from the SPI bus */
	return (uint8_t)SPI_I2S_ReceiveData(CC2500_SPI);
}

//#ifdef USE_DEFAULT_TIMEOUT_CALLBACK
/**
	* @brief  Basic management of the timeout situation.
	* @param  None.
	* @retval None.
	*/
uint32_t CC2500_TIMEOUT_UserCallback(void)
{
	/* Block communication and all processes */
	while (1)
	{   
	}
}
//#endif /* USE_DEFAULT_TIMEOUT_CALLBACK */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
