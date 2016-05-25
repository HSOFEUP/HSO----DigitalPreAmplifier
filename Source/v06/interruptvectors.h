#include <avr/interrupt.h>
#include "usart.h"


// prototypes to use
ISR(USART1_RX_vect);
ISR(USART0_RX_vect);

// e preciso colocar todas, senao nao inclui depois se usarmos