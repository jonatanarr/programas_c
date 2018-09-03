#include "MKL46Z4.h"
#include "board.h"

static int i = 0;

int main(void)
{
	board_init();
	PORTC_PCR3 |= PORT_PCR_IRQC(9);
	NVIC_SetPriority(PORTC_PORTD_IRQn, 0);
	NVIC_EnableIRQ(PORTC_PORTD_IRQn);
	//NVIC_IPR7|=0<<30;
	//NVIC_ISER|=1<<31;


    for (;;) {
        i++;
    }
    return 0;
}

void PORTC_PORTD_IRQHandler(){
	PORTC_PCR3|=PORT_PCR_ISF_MASK;
	LED_ROJO_TOGGLE;
}
