#include "MKL46Z4.h"

static int i = 0;

int main(void)
{
	int16_t lectura;
	double lectura_en_grados;

	SIM_SCGC6 |= 1<<27; // clock al modulo
	ADC0_CFG1 = 0x7D; // reg config del adc

    for (;;) {
    	ADC0_SC1A = 0x1A;
    	while  ((ADC0_SC1A & 0x80) == 0){}
    	lectura = ADC0_RA;
    	lectura_en_grados = 25 - (lectura*3.0/65535.0 -0.651)/0.00162;
    }

    return 0;
}
