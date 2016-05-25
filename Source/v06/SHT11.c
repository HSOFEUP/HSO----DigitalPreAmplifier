/*
* SHT11.c
*
* Created: 27-07-2015 17:32:57
* 
*/



// ctrl+D indent code
#include "SHT11.h"

/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 2000000UL
#endif



#include <avr/io.h>
#include <util/delay.h>
#include "bit_tools.h"


// temos que alterar isto
/************************************************************************/
/* @readSHT11					                                        */
/************************************************************************/
void readAllRegistersSHT11(float *temp, float *truehumid, uint8_t address){
	uint16_t restHumid=0;
	uint16_t resTemp=0;
	float realHumid;
	
	measureTempSHT11(&resTemp,address);// ja n me lembro pk foi feito duas
	// aqui podemos testar os ack e prosseguir
	measureHumidSHT11(&restHumid,address);
	
	//CHECK - this call arguments
	calcValuesSHT11(resTemp,restHumid, temp, &realHumid, truehumid);
	
}


/************************************************************************/
/* @calcData // receive two values and receives pointer memory to store stuff     */
/************************************************************************/
void calcValuesSHT11 (uint16_t temperature, uint16_t humid, float *tc, float *rhlinear, float *rhtrue){
	float rh;
	
	*tc = ( (float)temperature *0.01) -40.0; 
	rh =(float) humid;
	
	// Need to check this calcs #CHECK
	*rhlinear = (rh*0.0405) -(rh*rh*0.0000028) - 4.0;
	
	*rhtrue = ((*tc -25)*(0.01 + (0.00008*(rh))))+(*rhlinear);
	//:D
	// repara
	// recebo apontador, para aceder ao valor apontado tenho que descre um nivel ptr -> value//mas nao estas a mexer no valor do endereço?
	// nao o que recebo e o endereco :DPOIS, quero e aceder ao valor OK ENNTENDI
	// repara que um flat sao 4 bytes, um endreco e apens 1, senao ao chamar a funcao eram logo 16 clicos gastos so para chegar ate ao inicio
	// Address
	//        - Value ok k falta? isto vai dar mt k mexer 


}

/************************************************************************/
/* @ measureHumid                                                       */
/************************************************************************/
uint8_t measureHumidSHT11(uint16_t *data, uint8_t base_add){
	
	uint8_t ack;
	
	startComSHT11();
	ack = writeDataSHT11(0x05 |base_add);
	if(ack) return 0; // not ready
	waitComFreeSHT11(); // espera medida esteja pronta
	*data =  readDataSHT11();
	return 1;
}


/************************************************************************/
/* @measureTemp                                                         */
/************************************************************************/
uint8_t measureTempSHT11(uint16_t *data, uint8_t base_add){
uint8_t ack;
	
	
	startComSHT11();
	ack = writeDataSHT11(0x03 | base_add);
	if(ack == 1) return 0; // retorna false, nao esta pronto
	waitComFreeSHT11();
	data =(uint16_t *) readDataSHT11();
	return 1;
	
}

/************************************************************************/
/* @resetSHT11                                                          */
/************************************************************************/
void resetSHT11(void){
	
	resetComSHT11();
	_delay_ms(25);
}


/************************************************************************/
/* @waitComFree                                                         */
/************************************************************************/
void waitComFreeSHT11(void){
	uint16_t delay;
	
	SHT11_SDA_FLOAT;
	SHT11_CLK_LOW;
	_delay_us(1);
	for (delay=0; delay <35000; delay++){ // ensure 350ms delay or earlyer if ready
		if(!SHT11_READ_SDA) break;
		_delay_ms(10);
	}	
}



/************************************************************************/
/* @ resetCom                                                                     */
/************************************************************************/
void resetComSHT11(void){
	uint8_t i;
	
	SHT11_SDA_FLOAT;
	SHT11_CLK_LOW;
	_delay_us(1);
	for(i=0; i<9; i++){
		SHT11_CLK_HIGH;
		_delay_us(1),
		SHT11_CLK_LOW;
		_delay_us(1);
	}
	startComSHT11();
}


/************************************************************************/
/* @startCom                                                                     */
/************************************************************************/
void startComSHT11(void){
	// isto aqui tenho de ver o diagrama temporal, lol
	// CLK: 1 
	
	// Ensure initial conditions
	SHT11_SDA_FLOAT; // Put SDA in Float mode, one by default
	SHT11_CLK_LOW;
	_delay_us(1);
	
	// Issue start of transmission
	SHT11_CLK_HIGH; // comeca aqui
	_delay_us(1);
	SHT11_SDA_LOW; // zero SDA
	_delay_us(1);
	SHT11_CLK_LOW; // Issue CLK
	_delay_us(1);
	SHT11_CLK_HIGH; // 
	_delay_us(1);
	SHT11_SDA_FLOAT;// em float por defeito fica a 1, assim liberto a porta para ler pull up 
	_delay_us(1);
	SHT11_CLK_LOW;

}


/************************************************************************/
/* @SHT11WriteData                                                                      */
/************************************************************************/
uint8_t writeDataSHT11(uint8_t data){
	uint8_t i;
	uint8_t ack;
	
	for(i=0; i<8; i++){
		SHT11_SDA_LOW;
		if((data & 0x80) >0) SHT11_SDA_FLOAT; // Put SDA in Float mode == putting 1 
		else SHT11_SDA_LOW; // set zero
		_delay_us(1);
		SHT11_CLK_HIGH;
		_delay_us(1);
		data = data <<1;  // Next Bit MSB to LSB
		
		
	}
	// send ack
	SHT11_CLK_LOW;
	_delay_us(1);
	ack = SHT11_READ_SDA;
	SHT11_CLK_HIGH;
	_delay_us(1);
	SHT11_CLK_LOW;
	return ack;
	
	
}



/************************************************************************/
/* @ readSHT11                                                                      */
/************************************************************************/
uint16_t readDataSHT11(void){
	uint8_t i;
	uint16_t data=0;


	//-- 1. lets shift MSB out
	for (i=0; i<8; i++){
		data = data <<1;
		// give clock
		SHT11_CLK_HIGH;
		_delay_us(1); // wait set bit // change later according to device speed
		
		if(SHT11_READ_SDA >0) data |=0x01;
		else data |= 0x00;
		SHT11_CLK_LOW;
		_delay_us(1);
	}
	// Do some acknolade to device
	SHT11_SDA_LOW; // Put SDA High
	_delay_us(1);
	SHT11_CLK_HIGH; // Do clk
	_delay_us(1);
	SHT11_CLK_LOW; //
	_delay_us(1);
	SHT11_SDA_FLOAT; // Put SDA in Float mode

	// LSB
	for (i=0; i<8; i++){
		data = data <<1;
		// give clock
		SHT11_CLK_HIGH;
		_delay_us(1); // wait set bit // change later according to device speed
		
		if(SHT11_READ_SDA >0) data |=0x01;
		else data |= 0x00;
		SHT11_CLK_LOW;
		_delay_us(1);
	}

	//-- ACK the device
	SHT11_SDA_FLOAT; // Put SDA in Float mode // Ensure that SDA remains float to ensure state
	_delay_us(1);
	SHT11_CLK_HIGH; 
	_delay_us(1);
	SHT11_CLK_LOW; //
	_delay_us(1);


	//-- 2. return data
	return data;
}

