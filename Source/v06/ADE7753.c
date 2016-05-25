/*
 * ADE7753.c
 *
 * Created: 01-08-2015 17:00:38
 * 
 */ 
/* define CPU frequency in Mhz here if not defined in Makefile */
#include "ADE7753.h"

#ifndef F_CPU
#define F_CPU 2000000UL
#endif


#include <avr/io.h>
#include "bit_tools.h"
#include "SPI.h"



/************************************************************************/
/* @WRITE DATA ADE7753                                                    */
/************************************************************************/
void write16DataADE7753(uint8_t register, uint16_t command){
	ADE7753_CS_SELECT;
	//delay cycles 50ns
	SPI_transceiver(ADE7753_WRITE | register);
	//delay cycles 50ns
	SPI_transceiver((uint8_t)(command>>8));
	//delay cycles 50ns
	SPI_transceiver(command);
	//delay cyles 100ns
	ADE7753_CS_CLEAR;
}

void write8DataADE7753(uint8_t register, uint8_t command){
	ADE7753_CS_SELECT;
	//delay cycles 50ns
	SPI_transceiver(ADE7753_WRITE | register);
	//delay cycles 50ns
	SPI_transceiver((uint8_t)(command>>8));
	//delay cycles 50ns
	SPI_transceiver(command);
	//delay cyles 100ns
	ADE7753_CS_CLEAR;
}

/************************************************************************/
/* @READ DATA ADE7753                                                   */
/************************************************************************/
void readDataADE7753(uint8_t register){
	ADE7753_CS_SELECT;
	SPI_transceiver(ADE7753_READ | register);
	//delay
	//convem que retonrne algo... se e read...// como nao estava acabado... nao terminei.
	// mas convem acabar o lcd para depois usar a vontade, ainda esta assim mei a martelo
	//nao percebi nada, tenho k ver o .h 
}

/************************************************************************/
/* @CONFIG ADE7753                                                       */
/************************************************************************/
void configADE7753(uint8_t ch1off_set, uint8_t ch2off_set,uint8_t adjGain,uint8_t phaseCalib,uint16_t actPowOSCorr,uint16_t powGainAdj, uint8_t ActEnrgDiv, uint16_t cfreqDivNum, uint16_t cfreqDivDen,uint16_t ch1RMSOS, uint16_t ch2RMSOS, uint16_t sGain, uint16_t SenrgDiv, uint16_t nHalfLineCyc, uint16_t ZeroCrossTimeOut, uint8_t nLineCyc, uint8_t pkSagLvl, uint8_t ch1PkLvlThrs, uint8_t ch2PkLvlThrs){
	
	write8DataADE7753(ADE_REG_CH1OS, ch1off_set);
	write8DataADE7753(ADE_REG_CH2OS, ch2off_set);
	write8DataADE7753(ADE_REG_GAIN, adjGain);
	write8DataADE7753(ADE_REG_PHCAL, phaseCalib);
	write16DataADE7753(ADE_REG_APOS, actPowOSCorr);
	write16DataADE7753(ADE_REG_WGAIN, powGainAdj);
	write8DataADE7753(ADE_REG_WDIV, ActEnrgDiv);
	write16DataADE7753(ADE_REG_CFNUM, cfreqDivNum);
	write16DataADE7753(ADE_REG_CFDEN, cfreqDivDen);
	write16DataADE7753(ADE_REG_IRMSOS, ch1RMSOS);
	write16DataADE7753(ADE_REG_VRMSOS, ch2RMSOS);
	write16DataADE7753(ADE_REG_VAGAIN, sGain);
	write8DataADE7753(ADE_REG_VADIV, SenrgDiv);
	write16DataADE7753(ADE_REG_LINECYC, nHalfLineCyc);
	write16DataADE7753(ADE_REG_ZXTOUT, ZeroCrossTimeOut);
	write8DataADE7753(ADE_REG_SAGCYC, nLineCyc);
	write8DataADE7753(ADE_REG_SAGLVL, pkSagLvl);
	write8DataADE7753(ADE_REG_IPKLVL, ch1PkLvlThrs);
	write8DataADE7753(ADE_REG_VPKLVL, ch2PkLvlThrs);
	/*writeDataADE7753(TMODE, ???)*/
}
/************************************************************************/
/* @Set ADE7753 Interrupts                                              */
/************************************************************************/
void setAD7753_Interrupts(uint16_t command){
	write16DataADE7753(IRQEN,command);
	
}
/************************************************************************/
/* @Read ADE7753 Interrupt Status Register                              */
/************************************************************************/
void readADE7753_IntStatus(void){
int16_t temp;
	temp = readDataADE7753(0x0C);
	switch (temp){
		case: 0x01: // define estes num define, senao um gajo nunca sabe o que e// ok sao as fla
			SPI_transceiver(//Active Energy interrupt); //??
			break;	
		case: 0x02:
			SPI_transceiver(//no zero crossing or low thresh);
			break;
		case: 0x04:
			SPI_transceiver(//end of energy accumulation);
			break;
		case: 0x08:
			SPI_transceiver(//waveform register ready);
			break;
		case: 0x10:
			SPI_transceiver(//check);
			break;
		case: 0x20:
			SPI_transceiver(//Temperature conversion ready);
			break;
		case: 0x40:
			SPI_transceiver(//reset done);
			break;
		case 0x80:
			SPI_transceiver(//Active Energy regs OVF);
			break;
		case 0x100:
			SPI_transceiver(//Waveform channel2  exceeded VoltagePeakLevel VPKLV Value);
			break;
		case 0x200:
			SPI_transceiver(//waveform channel1 exceeded Current IPKLVL Value);
			break;
		case 0x400:
			SPI_transceiver(//Active energy register is more than half full VAENERGY);
			break;
		case 0x800:
			SPI_transceiver(//S energy (aparent) register has ovf'd);
			break;
		case 0x1000:
			SPI_transceiver(//missing zero crossing for specified number of line cycles);
			break;
		case 0x2000:
			SPI_transceiver(//Change of power from neg to pos);
			break;
		case 0x4000:
			SPI_transceiver(//change of power from pos to neg);
			break;
		default:
			break;
			
	}
}

/************************************************************************/
/* @ResetADE7753InterruptFlags									        */
/************************************************************************/
void RST_ADE7753_IntFlags(void){
	readDataADE7753(0x0B);
}

/************************************************************************/
/* @init ADE7753                                                        */
/************************************************************************/
void initADE7753(){
	ADE7753_POWER_ON;	//AFTER POWER ON ADE7753 INITS ON COMMUNICATIONS MODE
	write16DataADE7753(MODE,command);
}

/************************************************************************/
/* @CalcOnChipTemperature                                               */		//TO CHECK IF INTERRUPT IS DESIRED
/************************************************************************/
uint8_t calcOnChipTemp(void){
uint8_t onChipTemp;

	write16DataADE7753(MODE,(1<<5));
	_delay_us(26);						//CHECK WITH CLKIN - DELAY IS 96/CLKIN
	onChipTemp = readDataADE7753(TEMP);
	return onChipTemp;
}

