
#include <avr/io.h>
#include "bit_tools.h"


//CLK
#define SHT11_CLK_HIGH  bit_set(PORTC,0)
#define SHT11_CLK_LOW  bit_clear(PORTC,0)
// SDA
#define SHT11_READ_SDA bit_read(PORTC,1)
#define SHT11_SDA_HIGH bit_set(PORTC,1)
#define SHT11_SDA_LOW bit_clear(PORTC,1)
#define SHT11_SDA_FLOAT bit_set(PORTC,1)


//prototypes
void readAllRegistersSHT11(float *temp, float *truehumid, uint8_t address);
void calcValuesSHT11 (uint16_t temperature, uint16_t humid, float *tc, float *rhlinear, float *rhtrue);
uint8_t measureHumidSHT11(uint16_t *data, uint8_t base_add);
uint8_t measureTempSHT11(uint16_t *data, uint8_t base_add);
void resetSHT11(void);
void waitComFreeSHT11(void);
void resetComSHT11(void);
void startComSHT11(void);
uint8_t writeDataSHT11(uint8_t data);
uint16_t readDataSHT11(void);

