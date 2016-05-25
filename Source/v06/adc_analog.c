#include "adc_analog.h"

/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 2000000UL
#endif



#include <avr/io.h>
#include <util/delay.h>
#include "bit_tools.h"



/************************************************************************/
/* @obtain prescaler at ADc frequancy specified                                                                     */
/************************************************************************/
uint8_t ADC_calcPreScaler(uint8_t adcClk){ // ne precisa depois alterase

	// CHECK THIS
	
	if ((F_CPU / 2) < adcClk) return ADC_PRESC_DIV2;
	else if ((F_CPU / 4) < adcClk) return ADC_PRESC_DIV4;
	else if ((F_CPU / 8) < adcClk) return ADC_PRESC_DIV8;
	else if ((F_CPU / 16) < adcClk) return ADC_PRESC_DIV16;
	else if ((F_CPU / 32) < adcClk) return ADC_PRESC_DIV32;
	else if ((F_CPU / 64) < adcClk) return ADC_PRESC_DIV64;
	else if ((F_CPU / 128) < adcClk) return ADC_PRESC_DIV128;
	else return 0;
}



/************************************************************************/
/* @init ADC                                                                    */
/************************************************************************/
void ADC_init(uint8_t mode, uint8_t voltageRef,uint8_t adcClk){
	ADCSRA |=mode;
	ADCSRA |= ADC_calcPreScaler(adcClk);
	ADMUX = voltageRef;
	
}



/************************************************************************/
/* @ CHECK how to do best                                                                     */
/************************************************************************/
uint16_t ADC_readFromINT(void){
	
	uint16_t val;
	
	val = ADCL;
	val |= (ADCH <<8);
	// clear flags for next run
	
	return val;
}



/************************************************************************/
/* @fire ADC                                                                     */
/************************************************************************/
void ADC_startConversion(uint8_t ch){
	
	ch&= 0b00000111;
	ADMUX &= 0xF8 |ch;
	
	//-- Start conv
	ADCSRA |= ADC_START_CONV;
	
	// wait until complete
	while(ADCSRA & ADC_START_CONV);
	
}



/************************************************************************/
/* @adc check if conversion finished and read                                                                     */
/************************************************************************/
uint8_t ADC_checkStatusAndRead(uint16_t *val){
	
	// wait until complete
	if(ADCSRA & ADC_START_CONV) return 0;
	
	// grab values é ler pra uma var adcl e adch
	*val = ADCL;
	*val |= (ADCH <<8);
	return 1;
	
}


/************************************************************************/
/* @read adc                                                                     */
/************************************************************************/
uint16_t ADC_readAndWAIT(uint8_t ch){
uint16_t val;
	
	ch&= 0b00000111;  
	ADMUX &= 0xF8 |ch; 
	
	//-- Start conv
	ADCSRA |= ADC_START_CONV;
	
	// wait until complete
	while(ADCSRA & ADC_START_CONV);
	
	// grab values é ler pra uma var adcl e adch
	val = ADCL;
	val |= (ADCH <<8);
	return val;
	 
}