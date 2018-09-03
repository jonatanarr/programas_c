#include "MKL46Z4.h"
#include "board.h"


int main(void)
{
	uint8_t salida, valor;
	board_init();


    for (;;) {
    	ADC0_SC1A = 0x3; // status and control register - Selecciono el canal 0 DP3 del light sensor
        while ((ADC0_SC1A & 0x80) == 0) {} // acá hago un and bit a bit hasta que se levante la flag COCO
        valor = ADC0_RA;
    	//adc0_ra = data result register
        if(valor >=0 && valor <= 63){
        	salida = 3;
        	led_rojo(1);
        	led_verde(1);
        }
        if(valor >= 64 && valor <= 127){
        	salida = 2;
        	led_rojo(1);
        	led_verde(0);
        }
        if(valor >= 128 && valor <= 191){
        	salida = 1;
        	led_rojo(0);
        	led_verde(1);
        }
        if(valor >= 192 && valor <= 255){
        	salida = 0;
        	led_rojo(0);
        	led_verde(0);
        }


    }
    return 0;
}
