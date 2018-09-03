#include "MKL46Z4.h"

static int i = 0;

int main(void)
{

    /* Write your code here */

    /* This for loop should be replaced. By default this loop allows a single stepping. */
    for (;;) {
        i++;
    }
    /* Never leave main */
    return 0;
}

void init_reloj()
{

}
void init_tpm(){
//dar clock al modulo TMP0:
	SIM_SCGC6 = (1<<24);
// dar clk al modulo del port E (led rojo):
	SIM_SCGC5 = (1<<13);
// dar clk al modulo del port D (led verde)
	SIM_SCGC5 = (1<<12);

// seleccionar la funcion de los pines para TPM:
	PORTD_PCR5 |= PORT_PCR_MUX(4);
	PORTE_PCR29 |= PORT_PCR_MUX(3);

//verde
//rojo

	TPM0_C5SC |= 0x14;
	TPM0_C2SC |= 0x14;

	TPM0_C5V |= 16383;
	TPM0_C2V |= 32767;

	TPM0_MOD |= 32767;
	TPM0_SC |= (1<<3);

//seteo el prescaler: TPM0_SC ->PS
//programo los canales 2 y 5 como output-compare para hacer TOGGLE de la salidas: TPM0_CxSC
//verde
//rojo
//programo los valores de cuentas para el toggle: TPM0_CnV
//verde
//rojo
//programo el valor del modulo (para que vuelva a cero cuenado corresponde): TPM0_MOD
// seleccion en el TPM0 el reloj habilitado: TPM0_SC ->CMOD, con lo que el timer arranca
}
