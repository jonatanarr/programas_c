#include "fsl_device_registers.h"
#include "board.h"
/* DECLARACION DE VARIABLES */

_Bool E1,E2,E3,E4,E5,E6; /*ESTADOS*/
_Bool T1,T2,T3,T4,T5,T6; /*TRANSICIONES*/
_Bool P,R;

int main(void)
{

	/* INICIALIZACION DE GPIO */
	board_init();
	led_rojo(0);
	led_verde(0);

	/* MARCADO INICIAL */
	E1 = 1;
	E2 = 0;
	E3 = 0;
	E4 = 0;
	E5 = 0;
	E6 = 0;

	for (;;) {
		/* SE ADQUIEREN LAS ENTRADAS */
		P = pulsadorSw1_get();

		/* SE DETERMINA EL ESTADO DE LAS TRANSICIONES */
		T1 = E1 && P;
		T2 = E2 && !P;
		T3 = E3 && P;
		T4 = E4 && !P;
		T5 = E5 && P;
		T6 = E6 && !P;
		/* EVOLUCION DE LA RED SEGUN LAS TRANSICIONES */
		if (T1) {E1 = 0; E2 = 1;}
		if (T2) {E2 = 0; E3 = 1;}
		if (T3) {E3 = 0; E4 = 1;}
		if (T4) {E4 = 0; E5 = 1;}
		if (T5) {E5 = 0; E6 = 1;}
		if (T6) {E6 = 0; E1 = 1;}

		/* SETEO DE LAS SALIDAS */

		R = E5;
		if (R==1){LED_ROJO_ON;}
		else LED_ROJO_OFF;

    }
    /* Never leave main */
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
