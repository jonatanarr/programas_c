#include "MKL46Z4.h"
#include "board.h"


int main(void)
{
	uint8_t valor,SALIDA=0;
	board_init();



    for (;;) {


    	ADC0_SC1A = 0x3; // status and control register - Selecciono el canal 0 DP3 del light sensor
    	ADC0_CV1 = 0X80;
    	while ((ADC0_SC1A & 0x80) == 0) {} // acá hago un and bit a bit hasta que se levante la flag COCO
    	//adc0_ra = data result register
        if((ADC0_SC2 & (1<<4)) == 1){
        	led_rojo(1);
        	led_verde(1);
        	SALIDA =1;
        }
        if((ADC0_SC2 & (1<<4)) == 0){
        	led_rojo(0);
        	led_verde(0);
        	SALIDA = 0;
        }
    }
    return 0;
}
