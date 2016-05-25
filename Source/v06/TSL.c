/*
 * TSL.c
 *
 * Created: 28-07-2015 17:31:20
 *  
 */ 

#define F_CPU 20000000

#include <avr/io.h>
//#include <math.h>
#include <avr/delay.h>
#include <avr/interrupt.h>

/************************************************************************/
/* @LCD                                                                 */
/************************************************************************/

#include "lcd.h"
#include "bit_tools.h"
#include "twi.h"
#include "SPI.h"
#include "tsl2561.h"
#include "SHT25.h"
#include "SHT11.h"
#include "usart.h"
#include "interruptvectors.h"
#include "eeprom.h"
#include "adc_analog.h"



lcd_t lcdBuffer;

/***************************************/
/* @Global Variables                                                    */
/************************************************************************/
volatile char usartBuffer[USART_BUFFER_LENGHT];
volatile uint8_t nReceivedBytes=0;
volatile uint8_t flagNewMessage =0;

/************************************************************************/
/* @decode message                                                      */
/************************************************************************/
void decodeReceivedMessage(void){

uint8_t dummy;	

	
}


/************************************************************************/
/* @config ports                                                        */
/************************************************************************/
void configGPIO(void){
	// configurar 
	
	DDRC = 0xff;
	DDRB |= (1<<5);
}


/************************************************************************/
/* @ Define Timer1 Mode                                                 */
/************************************************************************/
#define TIMER1_MODE_NORMAL 0x00									//NORMAL
#define TIMER1_MODE_PWM_PHASECORR_8BIT (1<<WGM10)				//PWM PHASE CORRECT 8 BIT
#define TIMER1_MODE_PWM_PHASECORR_9BIT (1<<WGM11)				//PWM PHASE CORRECT 9 BIT
#define TIMER1_MODE_PWM_PHASECORR_10BIT (1<<WGM10) | (1<<WGM11)	//PWM PHASE CORRECT 10 BIT
#define	TIMER1_MODE_CTC (1<<WGM12)								//CTC
#define TIMER1_MODE_FASTPWM_8BIT (1<<WGM12) | (1<<WGM10)			//FAST PWM 8 BIT
#define TIMER1_MODE_FASTPWM_9BIT (1<<WGM12) | (1<<WGM11)			//FAST PWM 9 BIT
#define TIMER1_MODE_FASTPWM_10BIT (1<<WGM12) | (1<<WGM11) | (1<<WGM10)	//FAST PWM 10 BIT
#define TIMER1_MODE_PWM_PHASE_FREQ_CORRECT_ICR (1<<WGM13)				//PWM PHASE AND FREQUENCY CORRECT TOP ICR
#define TIMER1_MODE_PWM_PHASE_FREQ_CORRECT_OCRA (1<<WGM13) | (1<<WGM10)	//PWM PHASE AND FREQUENCY CORRECT TOP OCRA
#define TIMER1_MODE_PWM_PHASE_CORRECT_ICR (1<<WGM13) | (1<<WGM11)		//PWM PHASE CORRECT TOP ICR
#define TIMER1_MODE_PWM_PHASE_CORRECT_OCRA (1<<WGM13) | (1<<WGM11) | (1<<WGM10) //PWM PHASE CORRECT TOP OCRA
#define TIMER1_MODE_CTC (1<<WGM13) | (1<<WGM12)							//CTC MODE
#define TIMER1_MODE_FASTPWM_ICR (1<<WGM13) | (1<<WGM12) | (1<<WGM11)		//FAST PWM TOP ICR
#define TIMER1_MODE_FASTPWM_OCRA (1<<WGM13) | (1<<WGM12) | (1<<WGM11) |(WGM10)//FAST PWM TOP OCRA

/************************************************************************/
/* @DEFINE TIMER1 CLOCK AND EDGE                                        */
/************************************************************************/
#define TIMER1_NO_CLK 0x00
#define TIMER1_PRESC1  (1<<CS10)
#define TIMER1_PRESC8  (1<<CS11)
#define TIMER1_PRESC64  (1<<CS11) |  (1<<CS10)
#define TIMER1_PRESC256  (1<<CS12)
#define TIMER1_PRESC1024 (1<<CS12) | (1<<CS10)
#define TIMER1_EXT_CLK_EDGE_FALL (1<<CS12) | (1<<CS11)
#define TIMER1_EXT_CLK_EDGE_RISE (1<<CS12) | (1<<CS11) | (1<<CS10)

/************************************************************************/
/* DEFINE TIMER1 ENABLE INTERRUPT FLAGS                                 */
/************************************************************************/
#define TIMER1_INPUTCAPT_FLG_EN (1<<ICF1)
#define TIMER1_OUTPUT_COMPA_FLG_EN (1<<OCF1A)
#define TIMER1_OUTPUT_COMPB_FLG_EN (1<<OCF1B)


/************************************************************************/
/* @TIMER1 INTERRUPT MASK                                                */
/************************************************************************/
#define TIMER1_INPUTCAPT_INT_ENABLE (1<<TICIE1)
#define TIMER1_OUTPUT_COMPA_INT_ENABLE (1<<OCIE1A)
#define TIMER1_OUTPUT_COMPB_INT_ENABLE (1<<OCIE1B)
#define TIMER1_OVF_INT_ENABLE (1<<TOIE1)
#define TIMER1_OUTPUT_COMPC_INT_ENABLE (1<<OCIE1C)


/************************************************************************/
/* @timer config                                                        */
/************************************************************************/
void TIMER1_config(uint8_t config){

	TCCR1A |= 0; //CHECK
	
	
}
/************************************************************************/
/* @set pwm                                                                     */
/************************************************************************/
void PWM_init(void){
	
	
}


// e continuar
/************************************************************************/
/* @set dutycicle                                                                     */
/************************************************************************/
void PWM_OC1A_setDutyCycle(uint8_t dt){
	OCR1A = dt;
}

static ADC_convertVolts(uint16_t X){
	
	return (float)X*(float)(ADC_RESOLUTION)/(float)ADC_REF_VALUE; 
	 
}


//
int main(void){
uint16_t fullLightValue, irLightvalue, lux_value;
float  tempValue, rhValue, trueRhValue;	
uint8_t data = 0x02;			// bit 1 value must be 1 to avoid load default settings after each measuring command
uint8_t counter=0;
uint16_t dummy;
float volts;
	// Init Twi
	
	/*
	I2C_init();
	hardPowerOnTSL(TSL_ADDR,TSL_TINT_101MS);  	
	hardPowerONSHT25(SHT25_ADDRESS, SHT25_RH08_T12_BITS);
	data = readRegisterSHT25(SHT25_ADDRESS);
	*/
	//configGPIO();
	
	ADC_init(ADC_ENABLE , ADC_REF_INTERNAL_256,ADC_MAX_FREQ); 
		
	LCD_init();
	LCD_clr();
	LCD_setVisible();
	LCD_gotoXY(2,2);
	LCD_sendChar('H');
	
	lcdBuffer.nLines=LCD_LINES;
	lcdBuffer.lenght=LCD_LINE_LENGTH;	
	
	
	USART0_config(USART0_MY_UBBRN,USART_DATA_FORMAT_8BITS|USART_STOP_BITS_1,USART_TRANSMIT_ENABLE|USART_RECEIVE_ENABLE|USART_INTERRUPT_ENABLE);
	USART1_config(USART1_MY_UBBRN,USART_DATA_FORMAT_8BITS|USART_STOP_BITS_1,USART_TRANSMIT_ENABLE|USART_RECEIVE_ENABLE|USART_INTERRUPT_ENABLE);
	
	sei();
	
	USART0_sendStr("Hello");
    while(1){
		/*
		fullLightValue = readTSLCH0(TSL_ADDR);
		irLightvalue = readTSLCH1(TSL_ADDR);
		lux_value = calculateTSLLux(0,TSL_TINT_101MS,fullLightValue,irLightvalue,0);
		
		tempValue = calcTempSHT25(SHT25_ADDRESS,SHT25_T_TRIGGER_HOLDMASTER);
		rhValue = calcRHSHT25(SHT25_ADDRESS,SHT25_RH_TRIGGER_HOLDMASTER);
		trueRhValue = calcRHAbsoluteSHT25(tempValue,rhValue);
		
		//readRHSHT25(SHT25_ADDRESS, SHT25 );
		*/
		
		//fazias um clrscr
		//volts = ADC_convertVolts(ADC_read(0));
		dummy = ADC_readAndWAIT(0);
		sprintf(lcdBuffer.line3Data,"Value %4d \n\r",dummy);
		USART0_sendStr(lcdBuffer.line3Data);
		
		if (flagNewMessage > 0){
			USART0_sendStr("Got message");
			counter++;
			sprintf(lcdBuffer.line2Data,"N MSG RECIV %3d",counter);
			LCD_gotoXY(0,3);
			LCD_sendString(lcdBuffer.line2Data );
			
			USART1_sendStr(lcdBuffer.line2Data); 
			
			
			//decodeReceivedMessage();
			flagNewMessage=0;
			clearBuffer();
		}
	}
}						
			

