#include <16f877a.h>                   
#fuses HS,NOWDT,NOPROTECT,PUT,NOLVP 
#device adc=10                      //  A/D  de 10 bits
#use delay (clock=4000000)  
#use I2C(MASTER,FAST=100000, SDA=PIN_C4, SCL=PIN_C3,FORCE_HW)

// Debug code

// Includes
#include <stdio.h>
#include <stdlib.h>                      // Necessario para funcao atol ou atoi
#include "LCD.c"
#use fast_io(C)


// Defines
#define NPOTENTIOMETERS 4
#define SIZEBUFFER 16

// Base address for the PCF'S
// Read
#define PCFBUTTONADDRESS 0xFF // Check

// Write
#define PCFLEDBUTTONADDRESS 0x42
#define PCFLINERECADDRESS 0x40
#define PCFPHONOADDRESS
#define PCFLCDDDRESS

// Redifines Pin interrupt
#define BUTOONPRESSED PIN_B7


// Decode the buttons
#define LINEUP       0x01
#define LINEDOWN     0x02
#define LINEUPDOWN   0x03

#define RECUP        0x04
#define RECDOWN      0x08
#define RECUPDOWN     0x0C

#define DIRECT
#define LOUD


// Set Threshold for adc detections
#define ADCTHRESHOLDDETECTION 0x05

// Scheduler
#define SCHEDULINGTIMEPERIOD 10

// Macros for convertion
#define CONVERTADCTOPERCENTAGE(VAL)((VAL/1024)*100)
#define CONVERTADCTODB(VAL)(((VAL-512)/512)*12)

// Names for the devices
const char *nameRecLine[] = {"TAPE1","TAPE2","TAPE3","CD","TUNER","SLOT1","SLOT2","MUTE"};


// Global Variables
unsigned int16 bufferPotNew[NPOTENTIOMETERS];
unsigned int16 bufferPotOld[NPOTENTIOMETERS];

unsigned int8 pcfButton;      // Buffer for buttons
char buffer [SIZEBUFFER];
unsigned int8 flagKeyPressed, flagLCDUpdate;
unsigned int8 lineChannel, recChannel, ledButton;

int1 flagScheduling=0;    // Flag for sheduling scaler

unsigned int8 x,y;

// Prototipes
void tecla_pulsada(void);
unsigned int8 readPCFButtons(unsigned int8 address);
void decodePressedButton(unsigned int8 data);
void initLcd (void);
void long_delay_ms(int16 count);
void configGPIO(void);
void configADC(void);
void readADCS(void);
void checkADCChanges(void);
void setLineRecChannel(unsigned int8 address, unsigned int8 lineChannel,unsigned int8 recChannel, unsigned int8 recActive);
void updateLCD(char* buffer, unsigned int8 line);
void saveCurrentValues(void);
void restoreSavedValues(void);
void setPGAValue(unsigned int16 volume, unsigned int16 balance);
void setToneValue(unsigned int16 bass, unsigned int16 tremble);




/////////////////////INTERRUPT ZONE//////////////////////////
// Interrupt: Check witch button is pressed
#INT_RB
void tecla_pulsada(void){


   if(!(input_b()^0xF0))
      return;                  // Return: all interrupts cleared

   if((!input(BUTOONPRESSED)))
         {
            pcfButton=readPCFButtons(PCFBUTTONADDRESS);             // Pulser detected
            flagKeyPressed=1;
            return;
         }

}


// Interrup: Timer2 Interrupt
#INT_TIMER2 
void timer2_isr(void) { 
   //Keep variables that are local _local_ 
   static int8 tick; 
   if (tick == SCHEDULINGTIMEPERIOD){
      // clean the tick counter
      tick =0;
      // rise the flag for scheduling
      flagScheduling =TRUE;
   }
   tick++;
} 



////////////////FUNCTIONS ZONE////////////////////////////////
// Function: readPCF
unsigned int8 readPCFButtons(unsigned int8 address){
unsigned int8 data;
   // Disable all
   i2c_start ();
   i2c_write (address+1);
   data=i2c_read (0);
   i2c_stop();
   data=data&0x3F;   // mask the two higher bits
   return data;

}


// Function: decode the pressed key 
void decodePressedButton(unsigned int8 data){
unsigned int8 decode;

   decode=(data&0x0f);  // Clean Buttons (Apply at the end)
   switch(data){
   
      // LINE    
      case LINEUP:
         lineChannel++; // Move to main update
         setLineRecChannel(PCFLINERECADDRESS,lineChannel,recChannel,1); // Set the relay
         sprintf(buffer,"SRC: %s",nameRecLine[lineChannel]);
         
         flagLCDUpdate=1;
         break;
         
      case LINEDOWN:
         lineChannel--;
         setLineRecChannel(PCFLINERECADDRESS,lineChannel,recChannel,1); // Set the relay
         sprintf(buffer,"SRC: %s",nameRecLine[lineChannel]);
         flagLCDUpdate=1;
         break;
         
      case LINEUPDOWN:
         ledButton^= // Toggle the two leds and shut down output (toogle)
         flagLCDUpdate=1;
         break;
  
      // REC
      case RECUP:
         recChannel++;
         setLineRecChannel(PCFLINERECADDRESS,lineChannel,recChannel,1); // Set the relay
         sprintf(buffer,"REC: %s",nameRecLine[recChannel]);
         flagLCDUpdate=2;
         break;
         
      case RECDOWN:
         recChannel--;
         setLineRecChannel(PCFLINERECADDRESS,lineChannel,recChannel,1); // Set the relay
         sprintf(buffer,"REC: %s",nameRecLine[recChannel]);
         flagLCDUpdate=2;
         break;
      
      case RECUPDOWN:  // Toggle the two leds and shut down output (toogle)
         
         flagLCDUpdate=2;
         break;
  
      // Default Break
      default:
         break;
   
   }
   
   // Apply the LOUD or DIRECT
   

}


// Function: Present the user message to the user
void initLcd (void){

   lcd_init(); 
   delay_ms(2);      
   x=5;y=1;                            // Carregar Posicao
   lcd_gotoxy(x,y);                    // Mandar lcd Posicao
   printf(lcd_putc,"Audio"); // Escreve Lcd
   x=5;y=2;
   lcd_gotoxy(x,y);
   printf(lcd_putc,"PreAmp");
   delay_ms(2000);
   printf(lcd_putc,"\f" ) ;  
   x=5;y=1;
   lcd_gotoxy(x,y);
   printf(lcd_putc,"SOUND" ) ;
   x=3;y=2;
   lcd_gotoxy(x,y);
   printf(lcd_putc,"ENGINEERING" ) ;
   delay_ms(2000);
   printf(lcd_putc,"\f" ) ;            // Limpa
   x=1;y=1;
   lcd_gotoxy(x,y);

}


// Function: Delay Routine
void long_delay_ms(int16 count) { 
unsigned int16 k; 

for(k = 0; k < count; k++) 
    delay_ms(1); 
}


// Function: Config GPIO and modes
void configGPIO(void){

  // Define port mode
   set_tris_a(0b11111111);
   set_tris_b(0x11111111); 
   set_tris_c(0b00000000);
   set_tris_d(0b00000000);
}


// Funtion: Setup Analog converter
void configADC(void){
   setup_adc(ADC_CLOCK_DIV_32);   //configure analog to digiral converter
   setup_adc_ports(ALL_ANALOG);   //set pins AN0-AN7 to analog 

}


// Function: Read all the potenciometers channels (Called in the timer)
void readADCS(void){
unsigned int8 i;

   for(i=0;i<NPOTENTIOMETERS; i++){
   
      set_adc_channel(i);
      delay_us(10);
      bufferPotNew[i]=read_adc();
      delay_us(10);
   }
}


// Function: Check ADC changes to last values and flag updates and prepare lcd buffer to update
void checkADCChanges(void){
unsigned int8 i;

  
   // Check for all channels the ADC threshold
   for(i=0; NPOTENTIOMETERS; i++){
      if(abs(bufferPotNew - bufferPotOld)>ADCTHRESHOLDDETECTION){
         bufferPotOld[i] = bufferPotNew[i];  // Update buffer
         switch(i){
            case 0:
               sprintf(buffer,"VOL: %3d",(unsigned int8)(CONVERTADCTOPERCENTAGE(bufferPotNew[i])) );
               break;
               
            case 1:
               sprintf(buffer,"BAL: L%2d R%2d",(unsigned int8)(100- CONVERTADCTOPERCENTAGE(bufferPotNew[i])),(unsigned int8)CONVERTADCTOPERCENTAGE(bufferPotNew[i]));
               break;
               
            case 2:
               
               sprintf(buffer,"TREB: L%2d dB",(int8)CONVERTADCTODB(bufferPotNew[i]));
               break;
               
            case 3:
            
               sprintf(buffer,"BASS: L%2d dB",(int8)CONVERTADCTODB(bufferPotNew[i]));
               break;
               
         }     
         flagLCDUpdate=i+3;                    // Save the pot to update ,3,4,5,6
         
      }
   
   }
   
}


// Function: Set the pga values
void setPGAValue(unsigned int16 volume, unsigned int16 balance){

}


// Function: Set the Tones values
void setToneValue(unsigned int16 bass, unsigned int16 tremble){
 
 
}

// Function: Select the line and rec relays
void setLineRecChannel(unsigned int8 address, unsigned int8 lineChannel,unsigned int8 recChannel, unsigned int8 recActive){
unsigned int8 lineChannelDecoded, recChannelDecoded;
   // Translate to the channel
   
   
   // Disable all
   i2c_start ();
   i2c_write (address);
   i2c_write (0x00);
   i2c_write (recChannel);
   i2c_stop();

   // Small Delay
   delay_ms(500);
   
   // Enable the channel
   i2c_start ();
   i2c_write (address);
   i2c_write (lineChannel);
   if(recActive >0)i2c_write (recChannel);
   else i2c_write (0x00);
   i2c_stop();

}


// Function: Update lcd on changes
void updateLCD(char* buffer){

   switch(flagLCDUpdate){
   
      case 1:
         lcd_gotoxy(1,1);printf(lcd_putc,buffer);
         
         break;
         
      case 2:
         lcd_gotoxy(1,2);printf(lcd_putc,buffer);
         
         break;
         
      case 3:
         lcd_gotoxy(1,3);printf(lcd_putc,buffer);
         
         break;
          
      case 4:
         lcd_gotoxy(1,4);printf(lcd_putc,buffer);
         
         break;
         
         
      case 5:
         lcd_gotoxy(1,4);printf(lcd_putc,buffer);
         
         break;
         
      case 6:
         lcd_gotoxy(1,4);printf(lcd_putc,buffer);
         
         break;
         
      default:
         break;
   
   }
   flagLCDUpdate=0;  // Clear the flag on exit

}



// Function: Save all values (Pot, Channels, REC, Modes)
void saveCurrentValues(void){

   //write_eeprom(0x00,memChannel);
}


// Function: Restore saved values
void restoreSavedValues(void){
   //memChannel=read_eeprom(0x00);
   //channel=memChannel;
}


// Main Loop the 
void main(void){

   
   enable_interrupts(INT_RB);
   enable_interrupts(GLOBAL);
   configGPIO();
   port_b_pullups(TRUE);
   
   
  
   

   lcd_init();
   initLcd();  // Show Banner
   
   
   // Fill the lcd fields ( Static )
   lcd_gotoxy(1,1); printf(lcd_putc,"SRC:");
   lcd_gotoxy(1,2); printf(lcd_putc,"REC:");
   lcd_gotoxy(1,3); printf(lcd_putc,"VOL:");
   lcd_gotoxy(1,4); printf(lcd_putc,"BAL:");
   // Load channel from eeprom and put on channel
   restoreSavedValues();
   
   // Main loop
   while(true)
   {
      
      // Handle the pressed key
      if(flagKeyPressed){
         decodePressedButton(pcfButton);     // decode the key
         
         flagKeyPressed=0;       // Clear the flag
      }
      // Read the ADC for scheduling
      if(flagScheduling==TRUE){
         readADCS();
         checkADCChanges();
         
         flagScheduling=FALSE;   // Clear the flag
      }
      
      // Update the lcd information if flag enabled(flag remaisn with decode)
      if(flagLCDUpdate>0){
         
         if(flagLCDUpdate >3){ // Update PGA
            setPGAValue(bufferPotNew[0], bufferPotNew[1]);
            setToneValue(bufferPotNew[0], bufferPotNew[3]);
         
         }
      flagLCDUpdate=0;  // clear the update flag
      }
      
       
  
  }
}
   


