#include "system_config.h"
#include "stm32f4xx_conf.h"
#include "motor_interface.h"

/*	system_config.c contains all the GPIO and ADC initializations 
*   @param 	motor_init & gpio_unit 		 defined to initialize GPIO pins for motor (GPIOD 7) and LEDs (GPIOD 12,13,14,15)
*	@param 	adc_unit 					 defined to initialize definitions of ADC
*	@param 	system_init 				 will initialize all these methods
*/


void motor_init(GPIO_InitTypeDef* gpio_init_s) {
	gpio_init_s->GPIO_Pin = MOTOR_PIN; // Initializes motor pin
	GPIO_Init(GPIOD, gpio_init_s); // Initializes the GPIOB peripheral
}

void gpio_init(void) {
	GPIO_InitTypeDef gpio_init_s; // Structure to initilize definitions of GPIO
	GPIO_StructInit(&gpio_init_s); // Fills each GPIO_InitStruct member with its default value
	
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE); // Enables the AHB1 peripheral clock, providing power to GPIOB branch
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOD, ENABLE); // Enables the AHB1 peripheral clock, providing power to GPIOD branch
	gpio_init_s.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; // Select the following pins to initialise
	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT; // Operating mode = output for the selected pins
	gpio_init_s.GPIO_Speed = GPIO_Speed_100MHz; // Don't limit slew rate, allow values to change as fast as they are set
	gpio_init_s.GPIO_OType = GPIO_OType_PP; // Operating output type (push-pull) for selected pins
	gpio_init_s.GPIO_PuPd = GPIO_PuPd_NOPULL; // If there is no input, don't pull.
	GPIO_Init(GPIOD, &gpio_init_s); // Initializes the GPIOD peripheral.

	motor_init(&gpio_init_s);
}

void adc_init(void) {
	ADC_InitTypeDef adc_init_s;  // Structure to initialize definitions of ADC
	ADC_CommonInitTypeDef adc_common_init_s; // ADC Common Init structure definition
	ADC_DeInit(); // Deinitializes all ADCs peripherals registers to their default reset values.
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // Enable Power for bus APB2 connected to peripheral ADC1.
	adc_common_init_s.ADC_Mode = ADC_Mode_Independent; // Configures the ADC to operate in independent mode.
	adc_common_init_s.ADC_Prescaler = ADC_Prescaler_Div2; //Select the frequency of the clock to the ADC (devide clock frequency by 2). The clock is common for all the ADCs. Div2 means we count 0 and 1. Div4 means we count 0,1,2,3 on every clock cyle.
	adc_common_init_s.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //Configures the Direct memory access mode for multi ADC mode.
	adc_common_init_s.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; //Configures the Delay between 2 sampling phases.
	ADC_CommonInit(&adc_common_init_s); //Initializes the ADCs peripherals according to the specified parameters in the ADC_CommonInitStruct.

	adc_init_s.ADC_Resolution = ADC_Resolution_12b; // Configures the ADC resolution to 12 bits.
	adc_init_s.ADC_ScanConvMode = DISABLE; // Specifies whether the conversion is performed in 
	adc_init_s.ADC_ContinuousConvMode = DISABLE; 
	// Will just convert single value when triggered. If enabled it will start a new conversion as soon as it finishes one
	adc_init_s.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //Select the external trigger edge and enable the trigger of a regular group. Can set to rising, falling or both.
	adc_init_s.ADC_DataAlign = ADC_DataAlign_Right; // Specifies whether the ADC data  alignment is left or right. If right, MSB in register set to 0 and data is saved starting from LSB.
	adc_init_s.ADC_NbrOfConversion = 1; // Specifies the number of ADC conversions that will be done using the sequencer for regular channel group.!
	//wake up temperature sensor
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_Cmd(ADC1, ENABLE); // Enables the specified ADC peripheral (turn it on). 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles); // Configures for the selected ADC a regular channel, rank in the sequencer and its sample time. Channel is 16 for temperature sensor. 
	ADC_Init(ADC1, &adc_init_s);
}

void system_init(void) {
	gpio_init();
	adc_init();
}


