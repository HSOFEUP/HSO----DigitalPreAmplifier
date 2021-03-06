#include <16f877a.h> 
#device ADC=10               
#use delay (clock=4000000)
#use I2C(MASTER,FAST=100000, SDA=PIN_C4, SCL=PIN_C3,FORCE_HW)
//Libraries // Attention i2c must be defined after to make inclusion on the project
#include <stdlib.h>
#include <stdio.h>
#include <i2c_Flex_LCD.c>



// user defines

#fuses XT,NOPROTECT, NOPUT, NOWDT, NOBROWNOUT, NOLVP, NOCPD // configura fuses



/////////////////////////////////////
//////////// GLOBAL DEFINES////////

#define THRESHOLD_ANALOG 10   // Threshold value adc
#define NSAMPLES 10           // Number of samples to be taken each channel
#define NCHANNELS 4           // Number of channels to read
#define SCHEDULINGPERIOD 5    // 3*Base time of timer ex 10ms*3
#define LCD_SIZE 8           // lcd number colums

///////////////////////////////////////
///////////// Macros //////////////////
#define RESOLUTION 1024
#define MAX_DB 12.0

#define MACRO_PERCENTAGE (VARIABLE) ( (VARIABLE*100)/RESOLUTION)
#define MACRO_DB (VARIABLE) ( (VARIABLE - (RESOLUTION/2))*(RESOLUTION/2)/MAX_DB)
#define MACRO_BALANCE_LEFT (VARIABLE) ( (RESOLUTION/2) + ( (RESOLUTION/2) - (VARIABLE/2) ))
#define MACRO_BALANCE_RIGHT (VARIABLE)( (RESOLUTION/2) - ( (RESOLUTION/2) - (VARIABLE/2) ))

/////////////////////////////////////
//////////// ADDRESS DEFINES ////////

#define BUTTONS_ADDRESS  0x72
#define LEDS_ADDRESS   0x74
#define LCD_ADDRESS   0x40
#define SOURCE_ADDRESS  0x76
#define REC_ADDRESS   0x78
#define AUX_ADDRESS  0x80
#define TONE_ADDRESS   0x82





/////////////////////////////////////
//////////// STRUCTS////////


typedef struct{
   int16 analog_new[4];
   int16 analog_old[4];
   int1 flags[4];
   int1 update;
}analog_t;

typedef struct{
   char src[6];
   char rec[6];
   char vol[4];
   char bal[2][4];
   char tone [2][5];
   char loud [4];
   

}channel_t;

typedef struct{
   BYTE button_value_old;
   byte button_value_new;
   int1 flag;
   BYTE source;  // 00 off, 00000001 00000010 000001 etc direc to pcf
   BYTE rec;
   BYTE aux;
   
}pcf_t;

/////////////////////////////////////
//////////// GLOBAL VARIABLES////////

analog_t analog_values;    // Analog struct to save values;
int1 scheduling_flag=0;    // Flag for sheduling scaler
pcf_t state_pcf;


channel_t source;




/////////////////////////////////////
////////////// FUNCTIONS////////////

/////////////////////////////////////////////////////////////////////////
// Function to show splash message
void LCD_Splash ( unsigned char address)
{
char i=1;
         lcd_clear();  //Clear Display
         lcd_gotoxy(7,1);
         printf(LCD_PUTC,"%s","HSO");  //Print on LCD line 
         lcd_gotoxy(5,2);
         printf(LCD_PUTC,"%s","Pre V1.0");  //Print on LCD line
         delay_ms(1000);
         
         lcd_clear();  //Clear Display
         lcd_gotoxy(4,2);
         printf(LCD_PUTC,"%s","Warming Up");  //Print on LCD line 
         lcd_gotoxy(1,3);
         for (i=1;i<=LCD_SIZE;i++){
            lcd_gotoxy(i,3);
            printf(LCD_PUTC,"#");
            delay_ms(500);
         }
       
         
//!         lcd_backlight=OFF;
//!         printf(LCD_PUTC,"\f\1%s\2%s","TEST","     OFF      "); //Clear display, print again on Line 1
//!         delay_ms(1000);
//!         
//!         lcd_backlight=ON;
//!         printf(LCD_PUTC,"\f\1%s\2%s","TEST","     ON      "); //Clear display, print again on Line 2
//!         delay_ms(1000);
//!         
          lcd_clear();  //Clear Display
//!
}



/////////////////////////////////////////////////////////////////////////
// Function to Write to PCF
void PCF_8574_Write ( unsigned char data, BYTE address)
{
// Start i2c negotiation
        i2c_start();
        delay_us(20);
        i2c_write(address); //the slave addresse
        delay_us(20);
        i2c_write(data);
        delay_us(20);
        i2c_stop();
        delay_us(20);
   
   // perhaps here put an cicle to write all at once.
}



/////////////////////////////////////////////////////////////////////////
// Function to Read from PCF
char PCF_8574_Read (unsigned  char address)
{
char data =0;
   i2c_start();
   i2c_write(address+1);
   data = i2c_read(0);
   i2c_stop();
   return data;
   
}



/////////////////////////////////////////////////////////////////////////
// Function to config PGA2311
void PGA_2311_Config ()
{

}



/////////////////////////////////////////////////////////////////////////
// Function to Write value to PGA
void PGA_2311_Write ( unsigned char data_l, unsigned char data_r)
{

}



/////////////////////////////////////////////////////////////////////////
// Function to config the timer for the scheduling
void Timer_Config(void)
{
   //setup_timer_2(T2_DIV_BY_4,78,16);//setup up timer2 to interrupt every 1ms 
   setup_timer_2(T2_DIV_BY_16,125,5); //setup up timer2 to interrupt every 10ms 
  //setup_timer_2 (mode, period, post scale) 
  //mode - T2_DISABLED, T2_DIV_BY_1, T2_DIV_BY_4, T2_DIV_BY_16 
  //period is a int 0-255 that determines when the clock value is reset, 
  //post scale is a number 1-16 that determines how many timer overflows before 
  //an interrupt: (1 means once, 2 means twice, and so on). 
  
  // calculating timer reload value to generate 1 msec interrupt 
  // T = 1/f 
  // T = 1/4MHZ = 1/4* 1000000 
  // But Microcontroller hardware will divide the clock by 4 
  // and if we chosen T2_DIV_BY_4 then again there will 
  // one more division happen by 4 
  // So T = 1/(4 * 1000000)/ 4 * 4 /   T=4/4*1000000 
  // T = 4*4/4*1000000 = 0.000004 sec = 0.4 usec //4 usec 
  // ************************************************ 
  // At 4mhz, the timer will increment every 0.4 us // 4 usec *// 0.000004 sec 
  // ************************************************ 
  
  // ******************************************************** 
  // if period is chosen 250 then timer wi 
 //ll overflow every * 
  // 4 * 250 = 25 usec  // 1000 usec //1 msec                              * 
  // ******************************************************** 
  
  // ******************************************************************* 
  // And if we chosen post scalar as 5 then timer interrupt will occur * 
  // 25 usec  * 5 = 125 usec // 5000 usec /// 5 msec 
  // ******************************************************************* 

  enable_interrupts(INT_TIMER2);   // enable timer2 interrupt    
 
   
}



/////////////////////////////////////////////////////////////////////////
// Function to config the analog converter
void ADC_Config (void)
{
   setup_adc(ADC_CLOCK_DIV_32); //configure analog to digiral converter
   setup_adc_ports(ALL_ANALOG);   //set pins AN0-AN7 to analog 
}



/////////////////////////////////////////////////////////////////////////
// Function to check if the analog valuea have changed since last reading
void Analog_Check (void)
{
unsigned char i=0;
// lets check those that are dferent by some treshold and convert them
   for (i=0; i<NCHANNELS; i++){
      // If the value above threshold then change flag and refresh values old
      if ( abs(analog_values.analog_new[i] - analog_values.analog_old[i]) > THRESHOLD_ANALOG){
         analog_values.flags[i]= TRUE; // what is new?? 0,1,2,3
         analog_values.update=TRUE; // Flag that see if something new
      }
         
      // Copy value to the old
      analog_values.analog_old[i]= analog_values.analog_new;
      // Clean the new ones for new cycle
      analog_values.analog_new[i] =0;
   }

}



/////////////////////////////////////////////////////////////////////////
// Function to read all adc accorigi to the number defined
void ADC_Read_All (void)
{
unsigned char i,j=0;

   for (i=0; i<NCHANNELS ;i++){
      set_adc_channel(i);//set the pic to read from AN0 to an3
      delay_us(20);//delay 20 microseconds to allow PIC to switch to analog channel 0
            
      for (j=0; j<NSAMPLES; j++){
         analog_values.analog_new[i] = analog_values.analog_new[i] + read_adc();
      }
      // now get the mean value from samples
      analog_values.analog_new[i] /=NSAMPLES; 
            
   }
}





/////////////////////////////////////////////////////////////////////////
// VECTOR: External Interrupt
#INT_RB 
void ext_isr(void) { 
   
   // Enable the flag 
   state_pcf.flag = TRUE;
   
   // Disable the interrups
    disable_interrupts(INT_RB);
   
}




/////////////////////////////////////////////////////////////////////////
// VECTOR: Timer2 Interrupt
#INT_TIMER2 
void timer2_isr(void) { 
   //Keep variables that are local _local_ 
   static int8 tick; 
   if (tick == SCHEDULINGPERIOD){
      // clean the tick counter
      tick =0;
      // rise the flag for scheduling
      scheduling_flag =TRUE;
   }
   tick++;
} 

void Display_Update (unsigned char line)
{
   //0123456789012345//
   //SRC : TUNER     //
   //REC : TUNER  LD //
   //VOL : 100%   OFF//  
   //BAL : L50% R50% //
   //BASS : +12dB    //
   //TREM : +12db    //
   
   // Ok lest go
   switch (line){
   
      case 0x00:  lcd_gotoxy(1,1);
                  printf(LCD_PUTC("SRC : %5s",source.src));
                  lcd_gotoxy(2,1);
                  printf(LCD_PUTC("REC : %5s LOUD" ,source.rec));
                  lcd_gotoxy(3,1);
                  printf(LCD_PUTC("VOL : %3s%   %3s",source.vol,source.loud));
                  lcd_gotoxy(4,1);
                  printf(LCD_PUTC("BAL : L%2s R%2s",source.bal[1],source.bal[2]));
                  break;
                  
      case 0x01:  
                  break;
                  
      case 0x02:  
                  break;
                  
                  
                  
                  
   }
   
}

/////////////////////////////////////////////////////////////////////////
// MAIN
void main()
{
unsigned char i,dummy,test=0x00;


   state_pcf.flag=FALSE;

  lcd_init();
  ADC_Config();
  Timer_Config();
  PGA_2311_Config();
  
  lcd_backlight=ON;
  
  //Show splash message
  LCD_Splash (0x10);
  
  // Enable the interrups all
  enable_interrupts(INT_RB);

  ext_int_edge( H_TO_L );
  enable_interrupts(GLOBAL);
  state_pcf.button_value_new = PCF_8574_Read(BUTTONS_ADDRESS);
  
 
  // Main loop
  while (TRUE){  
  
      /////////////////////////////////////
      // Scheduling period task     
      if (scheduling_flag== TRUE){
                 
         // Read Analog Values
         ADC_Read_All();
         
         // Check for values that changed
         Analog_Check();
         
         // clean the schedule flag for next run
         scheduling_flag=FALSE;
      }
  
      
      
      
      ////////////////////////////////////////////////
      // On main loop chech for flags and update stuff
      ////////////////////////////////////////////////



      // Chech for changes of the pcf button after interrupt and debounce
      if (state_pcf.flag=TRUE){
         // lest read the pcf now
         state_pcf.button_value_new = PCF_8574_Read(BUTTONS_ADDRESS);
         
         // check whitch buttons changed 
         
         dummy =state_pcf.button_value_new;
         
         // clear the flag
         state_pcf.flag=FALSE;
         
         // Enable again the external interrupt for next reading 
         enable_interrupts(INT_RB);
   
      }
      
      
       // Check for ADC changes
       if(analog_values.update==TRUE){
          // we have something new loop all and refresh lcd and digital pots
          for (i=0; i< NCHANNELS; i++){
             if(analog_values.flags[i]==TRUE){
                // update the value on lcd
                // This is for the volume or balance already present on screen
                if(i<2){
                   // convert to percentage and send
                     
                }
                else{
                   // convert to bd and respfresh lcd an pots
                }
             }
            
            
          }
        }
      
   }   
} 
