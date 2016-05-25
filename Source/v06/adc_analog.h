#include <inttypes.h>
#include <avr/io.h>


#define ADC_ENABLE (1<<ADEN)
#define ADC_ENABLE_INT (1<<ADIE)
#define ADC_START_CONV (1<<ADSC)
#define ADC_PRESC_DIV2 (1<<ADPS0)
#define ADC_PRESC_DIV4 (1<<ADPS1)
#define ADC_PRESC_DIV8 (1<<ADPS0) | (1<<ADPS1)
#define ADC_PRESC_DIV16 (1<<ADPS2)
#define ADC_PRESC_DIV32 (1<<ADPS2) | (1<<ADPS0)
#define ADC_PRESC_DIV64 (1<<ADPS2) | (1<<ADPS1)
#define ADC_PRESC_DIV128 (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)
#define ADC_LEFT_JUSTIFIED (1<<ADLAR)
#define ADC_RIGHT_JUSTIFIED 0x00
#define ADC_REF_INTERNAL_256 (1<<REFS0) |(1<<REFS1)  
#define ADC_REF_VCC (1<<REFS0)
#define ADC_REF_EXTERNAL 0x00
#define ADC_MAX_FREQ 125000 //Khz
#define ADC_RESOLUTION 1024
#define ADC_REF_VALUE 5

// Macro functions
#define ADC_VOLTS(X) (((float)X*(float)ADC_RESOLUTION)/((float)ADC_REF_VALUE))



// prototipes
uint8_t ADC_calcPreScaler(uint8_t adcClk);
void ADC_init(uint8_t mode, uint8_t voltageRef,uint8_t adcClk);
uint16_t ADC_readFromINT(void);
void ADC_startConversion(uint8_t ch);
uint8_t ADC_checkStatusAndRead(uint16_t *val);
uint16_t ADC_readAndWAIT(uint8_t ch);
