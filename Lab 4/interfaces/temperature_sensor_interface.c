#include "stm32f4xx_conf.h"

#include "system_config.h"
#include "interfaces/temperature_sensor_interface.h"
#include "utils/ma_filter.h"


/*	temperature_sensor_interface.c is defined for all the following methods for temperature sensor:
*	temeperature_sensor_init				 	will initialize the ADC, time base configuration TIM4 which will be passed on to the processor through NVIC and to initialize the filter buffer
*	temperature_sensor_set_semaphore		 	will set the semaphore identity's value to switch between operations
*	temperature_sensor_read_temperature_raw	 	will enable ADC and convert digital signals to temperature form
*	temperature_sensor_read 				 	will pass the temperature values in the moving average filter to streamline the data
*   TIM4_IRQHandler 						 	interrupt handler, will reset semaphore to null once the operations are completed for that particular task
*/	

static MA_FILTER_TYPE buffer[MA_FILTER_DEPTH + 2];
static circular_buffer cb;
static ma_filter filter;

static float raw_reading;
static osSemaphoreId semaphore = NULL;


void temperature_sensor_init(void) {
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


	/* Time base configuration */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = TEMPERATURE_SENSOR_POLL_PERIOD_MS - 1;		//99
	TIM_TimeBaseStructure.dfgh  = TIM3_PRESCALER - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	/* TIM IT enable */
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	/* TIM4 enable counter */
	TIM_Cmd(TIM4, ENABLE);


	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the TIM4 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;							//passing on TIM4 as external interrupt to NVIC
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;			//third highest priority
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	ma_filter_initialize(&filter, &cb, buffer);
}

void temperature_sensor_set_semaphore(osSemaphoreId data_semaphore) {
	semaphore = data_semaphore;
}

void temperature_sensor_read_temperature_raw(void) {
	ADC_SoftwareStartConv(ADC1);
	//Starting Conversion, waiting for it to finish, clearing the flag, reading the result
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //Could be through interrupts (Later)
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	
	//EOC means End Of Conversion
	uint16_t result = ADC_GetConversionValue(ADC1); // Result available in ADC1->DR
	raw_reading = ((((float)(result * 3000)) / 4095) - V_25) / AVERAGE_SLOPE + 25;	//will convert result into degrees (formula given in the reference manual)
}

void temperature_sensor_read(float* temperature) {
	*temperature = ma_filter_add(&filter, raw_reading);
}



/*	external interrupt handler
*	will reset the semaphore to NULL once the temperature sensor operation is completed
*/
void TIM4_IRQHandler(void) {
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if (semaphore != NULL) {
			osSemaphoreRelease(semaphore);
		}
	}
}

