
#include "MKL46Z4.h"
#include "board.h"

static int i = 0;

int main(void)
{

	board_init();
	NVIC_SetPriority(ADC0_IRQn, 0);
	NVIC_EnableIRQ(ADC0_IRQn);

	for (;;) {i++;}
    return 0;
}

void ADC0_IRQHandler ()
{
		uint16_t valor, salida;
		ADC0_SC1A |= ADC_SC1_ADCH_MASK; //apago el conversor para que no siga convirtiendo


	 	 valor = ADC0_RA; //adc0_ra = data result register
	     if(valor >=0 && valor <= 16383)
	     {
	       	salida = 3;
	       	led_rojo(1);
	       	led_verde(1);
	     }
	     if(valor >= 16384 && valor <= 32767)
	     {
	       	salida = 2;
	       	led_rojo(0);
	       	led_verde(1);
	     }
	     if(valor >= 32768 && valor <= 49151)
	     {
	      	salida = 1;
	       	led_rojo(1);
	       	led_verde(0);
	     }
	     if(valor >= 49152 && valor <= 65535)
	     {
	      	salida = 0;
	       	led_rojo(0);
	       	led_verde(0);
	     }
	     ADC0_SC1A = 0x43;
}
