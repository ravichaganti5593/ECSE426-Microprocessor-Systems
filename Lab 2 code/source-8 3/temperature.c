#include "temperature.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

/*	temperature.c will enable ADC and then convert the digital signal to temperature
*	@param 	read_temperature 			method to convert ADC signal in the form of temperature		
*/

float temperature_reading;

void read_temperature(void) {
	ADC_SoftwareStartConv(ADC1);
	//Starting Conversion, waiting for it to finish, clearing the flag, reading the result
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //Could be through interrupts (Later)
	//EOC means End Of Conversion
	float result = ADC_GetConversionValue(ADC1); // Result available in ADC1->DR
	temperature_reading = (((result * 3000) / 4096) - V_25) / AVERAGE_SLOPE + 25;	//will convert result into degrees (formula given in the reference manual)
}