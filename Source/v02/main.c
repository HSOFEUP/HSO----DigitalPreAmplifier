#include <16F84A.h>

#FUSES NOWDT                 	//No Watch Dog Timer
#FUSES XT                    	//High speed Osc (> 4mhz)
#FUSES NOPUT                 	//No Power Up Timer
#FUSES NOPROTECT             	//Code not protected from reading

#use delay(clock=10000000)

#use delay(clock=10000000)
#use fast_io(B)

// Prototipes
void configPorts(void);
void setupTimer(void);
void setInitialState(void);
void readAD(void);
void writeAD(unsigned int16 value);
void startTimer(void);
void stopTimer(void);


// Global Variables
unsigned int16 tempValue;
unsigned int16 outValue;
int8 timerCount;

//Macros

#define N_BITS_SDA_TO_SHIFT 12
#define N_TIMER_RELOADS 11

// AD_CLCK_HIGH
// AD_CLOCK_LOW
// AD_DATA_READ_BIT
// AD_START_CONV
// AD_CS_LOW
// AD_CS_HIGH


// AD
#define AD_CLK_HIGH output_high(PIN_B7)
#define AD_CLK_LOW output_low(PIN_B7)

#define AD_START_CONV_HIGH output_high(PIN_B5)
#define AD_START_CONV_LOW output_low(PIN_B5)

#define AD_READ_SDI input(PIN_B6)

//DA
#define DA_CLK_LOW output_low(PIN_B4)
#define DA_CLK_HIGH output_high(PIN_B4)

#define DA_LOAD_LOW output_low(PIN_B1)
#define DA_LOAD_HIGH output_high(PIN_B1)

#define DA_CLR_LOW output_low(PIN_B3)
#define DA_CLR_HIGH output_high(PIN_B3)

#define DA_SDA_PIN_HIGH output_high(PIN_B2)
#define DA_SDA_PIN_LOW output_low(PIN_B2)

#define LED_OUT_HIGH output_high(PIN_B0)
#define LED_OUT_LOW output_low(PIN_B0)



/*
PIN MAP
RB0 = LEDS_OSC -OUT
RB1 = DA_LOAD  -OUT
RB2 = DA_DATA  -OUT
RB3 = DA_CLR   -OUT
RB4 = DA_CLK   -OUT

RB5 = AD_START_CONV -OUT
RB6 = AD_DATA -IN
RB7 = AD_CLK - OUT
*/

/*
--------------
ISR ZONE
--------------
*/
// (clock/4)/RTCC_DIV = Tempo overflow para a escala toda do timer, tem que ser 128, despois decontar o load,
// ou 256 e descontar o reload
// com 128, basta agora escolher um e calcular o reload, :D
// Eu tou a ver mas o datasheet nao da essa formula, esquecime dos 256, mas ok, e facil
#int_RTCC
void timer0_isr(void){

	// Reload timer on entry (less overhead)
	set_timer0(235);

	if (timerCount >0) LED_OUT_LOW;
	else LED_OUT_HIGH;

	timerCount++;
	if(timerCount >21){
 		timerCount=0;
		LED_OUT_HIGH;
 	}


}
// 




/*
--------------
FUNCTIONS ZONE
--------------
*/
// FUNCTION: CONFIGURE PORTS
void configPorts(void){
   set_tris_b(0b01000000);
}







// FUNCTION: READ ADC VALUES
void readAD(void){
int16 i=0;

	AD_CLK_LOW; // Ensure clk is low
    // Do a Start Conversion
    AD_START_CONV_LOW; // Start AD
	delay_us(1);
    AD_START_CONV_HIGH; //
    delay_us(8);


    //delay_cycles( 1 ); cedance rules may not be as intended, use () t// Same as NOP 1/5MHZ delay

    // lets read the stuff
    for (i=0; i<=15; i++){
       AD_CLK_HIGH;
	   if(AD_READ_SDI) tempValue |=1;
	   else tempValue |=0;
       AD_CLK_LOW;
	   tempValue <<=1;
    }
   
}



// FUNCTION: write AD
void writeDA(unsigned int16 value){
int16 i=0;


// ensure LOAD and CLK is High
DA_CLK_HIGH;
DA_LOAD_HIGH;

// Pre shift value to align output
value <<=( 4);


	for (i=0; i<=N_BITS_SDA_TO_SHIFT; i++){
    	DA_CLK_LOW; // lower clk
		if((value & 0x8000)==0){
			DA_SDA_PIN_LOW;
		}
		else {
			DA_SDA_PIN_HIGH;
		}
		// SDA is set now lets do clock

		// Issue clock and make bit out
		DA_CLK_HIGH;
		value <<=1; // Shift to left the bit
		
   	}
   
	
    // Issue Load values to DA
	DA_LOAD_LOW;
	DA_LOAD_HIGH;


}



// FUNCTION: SET TIMER
void setupTimer(void){

setup_timer_0(RTCC_INTERNAL|RTCC_8_BIT);
set_timer0(235);
enable_interrupts(INT_RTCC);


}

// FUNCTION: FIRE TIMER
void startTimer(void){
enable_interrupts(GLOBAL);

}

// FUNCTION: STOP TIMER
void stopTimer(void){
disable_interrupts(GLOBAL);
// Ensure Led stay //tenho esses valores 

}


// FUNCTION: SET InITIAL STATE
void setInitialState(void){

DA_CLK_HIGH;
// Do a reset DA
DA_LOAD_HIGH;
// issue reset
DA_CLR_LOW;
DA_CLR_HIGH;

AD_CLK_LOW;
AD_START_CONV_HIGH;

}




//--MAIN--
void main(){

//int8 dummy;

	configPorts();
	setupTimer(); // Apenas configurar timer para a freq e ver onde se pode lancar
	setInitialState();
	
	// Se estiver sempre a correr, podemos fazer isto
	startTimer(); // fica sempre a correr

  	while(1){
   		readAD();
   		// do calculations
   		//delay_ms(500); // Erase at the end this line
		outValue = tempValue; // Do the required math 
	   // writeDA(outValue); 
		writeDA(0x5555);
   
    }
}
