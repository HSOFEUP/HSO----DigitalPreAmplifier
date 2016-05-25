#include <avr/io.h>
#include "bit_tools.h"
#include "SPI.h"




/************************************************************************/
/*@Macros                                                               */
/************************************************************************/
#define ADE7753_CS_SELECT bit_clear(PORTB,0)
#define ADE7753_CS_CLEAR bit_set(PORTB,0)
#define ADE7753_WRITE 0x80
#define ADE7753_READ 0x00

/************************************************************************/
/* ADE7753 Power management                                             */
/************************************************************************/
#define ADE7753_POWER_ON bit_set(PORTE,4)
#define ADE7753_POWER_DOWN bit_clear(PORTE,4)

/************************************************************************/
/* ADE7753 REGISTER  ADRESSES                                           */
/************************************************************************/
#define ADE_REG_WAVEFORM 0x01 //This read-only register contains the sampled waveform data from either Channel 1, Channel 2, or the active power signal.
					  // The data source and the length of the waveform registers are selected by data Bits 14 and 13 in the mode register
#define ADE_REG_AENERGY 0x02  //Active power is accumulated (integrated) over time in this 24-bit, read-only register
#define ADE_REG_RAENERGY 0x03 //Same as the active energy register except that the register is reset to 0 following a read operation.
#define ADE_REG_LAENERGY 0x04 //The instantaneous active power is accumulated in this read-only register over the LINECYC number of half line cycles.
#define ADE_REG_VAENERGY 0x05 //Apparent power is accumulated over time in this read-only register.
#define ADE_REG_RVAENERGY 0x06 //Same as the VAENERGY register except that the register is reset to 0 following a read operation.
#define ADE_REG_LVAENERGY 0x07 //The instantaneous real power is accumulated in this read-only register over the LINECYC number of half line cycles.
#define ADE_REG_LVARENERGY 0x08 //The instantaneous reactive power is accumulated in this read-only register over the LINECYC number of half line cycles.
#define ADE_REG_MODE 0x09		//This is a 16-bit register through which most of the ADE7753 functionality is accessed.
						// Signal sample rates, filter enabling, and calibration modes are selected by writing to this register.
#define ADE_REG_IRQEN 0x0A		//ADE7753 interrupts can be deactivated by setting the corresponding bit in this 16- bit enable register to Logic 0. 
						//The status register continues to register an interrupt event even if disabled.
#define ADE_REG_INT_STATUS 0x0B	//Interrupt Status Register.This is an 16-bit read-only register. 
					//The status register contains information regarding the source of ADE7753 interrupts
#define ADE_REG_RSTSTATUS 0x0C //Same as the interrupt status register except that the register contents are reset to 0 (all flags cleared)
					   // after a read operation.
#define ADE_REG_CH1OS 0x0D	//Channel 1 Offset Adjust. Bit 6 is not used. Writing to Bits 0 to 5 allows offsets on Channel 1 to be removed
					//Writing a Logic 1 to the MSB of this register enables the digital integrator on Channel 1, a Logic 0 disables the integrator. 
					//The default value of this bit is 0.
#define ADE_REG_CH2OS 0x0E //Channel 2 Offset Adjust. Bits 6 and 7 are not used. Writing to Bits 0 to 5 of this register 
				   //allows any offsets on Channel 2 to be removed.Note that the CH2OS register is inverted. 
				   //To apply a positive offset, a negative number is written to this register.
#define ADE_REG_GAIN 0x0F  //PGA Gain Adjust. This 8-bit register is used to adjust the gain selection for the PGA in Channels 1 and 2
#define ADE_REG_PHCAL 0x10 //Phase Calibration Register. The phase relationship between Channel 1 and 2 can be adjusted by writing to this 6-bit register.
				   // The valid content of this twos compliment register is between 0x1D to 0x21. 
				   //At a line frequency of 60 Hz, this is a range from –2.06° to +0.7°
#define ADE_REG_APOS 0x11 //Active Power Offset Correction. This 16-bit register allows small offsets in the active power calculation to be removed
#define ADE_REG_WGAIN 0x12 //Power Gain Adjust. This is a 12-bit register. The active power calculation can be calibrated by writing to this register. 
				   //The calibration range is ±50% of the nominal full-scale active power. The resolution of the gain adjust is 0.0244%/LSB
#define ADE_REG_WDIV 0x12  //Active Energy Divider Register. The internal active energy register is divided by the value of this register
				   // before being stored in the AENERGY register.
#define ADE_REG_CFNUM 0x14 //CF Frequency Divider Numerator Register. 
				   //The output frequency on the CF pin is adjusted by writing to this 12-bit read/write register
#define ADE_REG_CFDEN 0x15 //CF Frequency Divider Denominator Register. 
				   //The output frequency on the CF pin is adjusted by writing to this 12-bit read/write register
#define ADE_REG_IRMS 0x16  //Channel 1 RMS Value (Current Channel).
#define ADE_REG_VRMS 0x17  //Channel 2 RMS Value (Voltage Channel).
#define ADE_REG_IRMSOS 0x18//Channel 1 RMS Offset Correction Register.
#define ADE_REG_VRMSOS 0x19//Channel 2 RMS Offset Correction Register.
#define ADE_REG_VAGAIN 0x1A//Apparent Gain Register. Apparent power calculation can be calibrated by writing to this register. 
				   //The calibration range is 50% of the nominal full-scale real power. The resolution of the gain adjust is 0.02444%/LSB.
#define ADE_REG_VADIV 0x1B //Apparent Energy Divider Register. 
				   //The internal apparent energy register is divided by the value of this register before being stored in the VAENERGY register.
#define ADE_REG_LINECYC 0x1C //Line Cycle Energy Accumulation Mode Line-Cycle Register. 
					 //This 16-bit register is used during line cycle energy accumulation mode to set the number of half line cycles for energy accumulation
#define ADE_REG_ZXTOUT 0x1D //Zero-Crossing Timeout. If no zero crossings are detected on Channel 2 within a time period specified by this 12-bit register, 
					//the interrupt request line (IRQ) is activated
#define ADE_REG_SAGCYC 0x1E //Sag Line Cycle Register. This 8-bit register specifies the number of consecutive line cycles the signal on Channel 2 
					//must be below SAGLVL before the SAG output is activated
#define ADE_REG_SAGLVL 0x1F//Sag Voltage Level. An 8-bit write to this register determines at what peak signal level on Channel 2 the SAG pin becomes active. 
				   //The signal must remain low for the number of cycles specified in the SAGCYC register before the SAG pin is activated
#define ADE_REG_IPKLVL 0x20//Channel 1 Peak Level Threshold (Current Channel). This register sets the level of the current peak detection. 
				   //If the Channel 1 input exceeds this level, the PKI flag in the status register is set.
#define ADE_REG_VPKLVL 0x21//Channel 2 Peak Level Threshold (Voltage Channel). This register sets the level of the voltage peak detection. 
				   //If the Channel 2 input exceeds this level, the PKV flag in the status register is set.
#define ADE_REG_IPEAK 0x22//Channel 1 Peak Register. The maximum input value of the current channel since the last read of the register
				  // is stored in this register.
#define ADE_REG_RSTIPEAK 0x23//Same as Channel 1 Peak Register except that the register contents are reset to 0 after read.
#define ADE_REG_VPEAK 0x24 //Channel 2 Peak Register
				   // The maximum input value of the voltage channel since the last read of the register is stored in this register.
#define ADE_REG_RSTVPEAK 0x25//Same as Channel 2 Peak Register except that the register contents are reset to 0 after a read.
#define ADE_REG_TEMP 0x26 //Temperature Register. This is an 8-bit register which contains the result of the latest temperature conversion
#define ADE_REG_PERIOD 0x27//Period of the Channel 2 (Voltage Channel) Input Estimated by Zero-Crossing Processing. 
				   //The MSB of this register is always zero.
#define ADE_REG_TMODE 0x3D //Test Mode Register.
#define ADE_REG_CHKSUM 0x3E//Checksum Register. This 6-bit read-only register is equal to the sum of all the ones in the previous read.
#define ADE_REG_DIEREV 0x3F //Die Revision Register. This 8-bit read-only register contains the revision number of the silicon.

/************************************************************************/
/* @INTERRUPT STATUS REGISTER                                           */
/************************************************************************/
#define ADE_INTF_ACT_ENERGY (1<<0)	//Indicates that an interrupt occurred because the active energy register, AENERGY, is more than half full.
#define ADE_INTF_SAG (1<<1)			//Indicates that an interrupt was caused by a SAG on the line voltage.
#define ADE_INTF_CYC_END (1<<2)		//Indicates the end of energy accumulation over an integer number of half line cycles as defined by the content of the LINECYC register
#define ADE_INTF_WAVE_SAMPLE (1<<3)	//Indicates that new data is present in the waveform register.
#define ADE_INTF_ZEROCROSS (1<<4)	//This status bit is set to Logic 0 on the rising and falling edge of the the voltage waveform.
#define ADE_INTF_TEMP_READY (1<<5)	//Indicates that a temperature conversion result is available in the temperature register.
#define ADE_INTF_RST_DONE (1<<6)		//Indicates the end of a reset (for both software or hardware reset).
#define ADE_INTF_ACT_ENERGY_OVF (1<<7) //Indicates that the active energy register has overflowed.
#define ADE_INTF_PEAK_VOLT (1<<8)	  //Indicates that waveform sample from Channel 2 has exceeded the VPKLVL value.
#define ADE_INTF_PEAK_CURR (1<<9)	´ //Indicates that waveform sample from Channel 1 has exceeded the IPKLVL value.
#define	ADE_INTF_S_ENERGY_HF (1<<10)   //Indicates that an interrupt occurred because the active energy register, VAENERGY, is more than half full.
#define	ADE_INTF_S_ENERGY_OVF (1<<11)  //Indicates that the apparent energy register has overflowed
#define ADE_INTF_ZEROCROSS_TMOUT (1<<12)//Indicates that an interrupt was caused by a missing zero crossing on the line voltage for the specified number of line cycles
#define ADE_INTF_P_POS (1<<13)			//Indicates that the power has gone from negative to positive.
#define ADE_INTF_P_NEG (1<<14)			//Indicates that the power has gone from positive to negative.


/************************************************************************/
/* @EEPROM STORE ON /READ FROM                                          */
/************************************************************************/

/************************************************************************/
/* @ ADE7753 Serial Interface is SPI COMPATIBLE                         */
/************************************************************************/
//CS MUST BE DRIVEN LOW FOR THE ENTIRE DATA TRANSFER
//1ST COMMAND BYTE--WAIT--MSB--WAIT--LSB
//1ST BIT MSB=1 > WRITE MSB=0 >READ
// 



void write16DataADE7753(uint8_t register, uint16_t command);
void configADE7753(uint8_t ch1off_set, uint8_t ch2off_set,uint8_t adjGain,uint8_t phaseCalib,uint16_t actPowOSCorr,uint16_t powGainAdj, uint8_t ActEnrgDiv, uint16_t cfreqDivNum, uint16_t cfreqDivDen,uint16_t ch1RMSOS, uint16_t ch2RMSOS, uint16_t sGain, uint16_t SenrgDiv, uint16_t nHalfLineCyc, uint16_t ZeroCrossTimeOut, uint8_t nLineCyc, uint8_t pkSagLvl, uint8_t ch1PkLvlThrs, uint8_t ch2PkLvlThrs);
void setAD7753_Interrupts(uint16_t command);
void readADE7753_IntStatus(void);
void RST_ADE7753_IntFlags(void);
void initADE7753();
uint8_t calcOnChipTemp(void);
