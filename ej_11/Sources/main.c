
#include "MKL46Z4.h"
#include "board.h"


int main(void)
{
	uint16_t lectura=0, salida=0;
	int j;
	board_init();

    while (1) {
    	while ((ADC0_SC1A & (1<<7)) == 0){}
    	lectura = ADC0_RA-8;
    	for(j=0; j<lectura; j++){led_verde(1);}
    	for(j=0; j<65535-lectura;j++){led_verde(0);}
    }
    return 0;
}
