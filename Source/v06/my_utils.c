
// ctrl+D indent code
#include "my_utils.h"

/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 2000000UL
#endif



#include <avr/io.h>
#include <util/delay.h>
#include "bit_tools.h"



/************************************************************************/
/* @UsartSendDecimal                                                    */
/************************************************************************/
void sendDecUSART(uint8_t value){
	USART0_sendChar(value + '0');
	// usase a default, para isto,
	
}

/************************************************************************/
/* @UsartSendHex                                                    */
/************************************************************************/
void sendHexUSART(uint8_t value){
	uint8_t nibble_low = value;
	uint8_t nibble_high = (value>>4);
	calc_hex_nibble(nibble_low);
	calc_hex_nibble(nibble_high);

}

/************************************************************************/
/* @CalcHexNibble                                                       */
/************************************************************************/
void calc_hex_nibble(uint8_t nibble){
	uint8_t maskednibble = (nibble & 0x0f);
	if (maskednibble>9) maskednibble += ('a'-10);
	else maskednibble +='0';
	
	USART0_sendChar(maskednibble);
}

/************************************************************************/
/* @Usart Send Binary                                                   */
/************************************************************************/
void sendBinUSART(uint8_t value){
	uint8_t i;
	
	for (i=128;i>0;i/2){
		if(value&i) USART0_sendChar('1');
		else USART0_sendChar('0');
		
	}
}