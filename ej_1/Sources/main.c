#include "fsl_device_registers.h"
#include "board.h"
/* DECLARACION DE VARIABLES */

_Bool E1,E2,E3,E4,R,ENT1;
_Bool T1,T2,T3,T4;


int main(void)
{
	/*inicializar los puertos*/
	board_init();
	led_rojo(0);
	led_verde(0);
    /* Marcado inicial*/
	E1=1;
	E2=0;
	E3=0;
	E4=0;


    for (;;) {
    	/*SE ADQUIEREN LAS ENTRADAS*/
    	ENT1=pulsadorSw1_get();

    	/*SE DETERMINAN LAS TRANSICIONES*/

    	T1 = E1 && ENT1;
    	T2 = E2 && !ENT1;
    	T3 = E3 && ENT1;
    	T4 = E4 && !ENT1;


    	/* EVOLUCION DE LA RED, SEGUN LAS REGLAS DE MARCADO*/

    	if (T1) {E1 = 0; E2 = 1;}
    	if (T2) {E2 = 0; E3 = 1;}
    	if (T3) {E3 = 0; E4 = 1;}
    	if (T4) {E4 = 0; E1 = 1;}

    	/* PRODUCCION DE LAS SALIDAS */

		R = E2 && ENT1 || E3 && !ENT1;

		/* FUNCION PARA PRENDER EL LED */

		if (R==1){LED_ROJO_ON;}
		else LED_ROJO_OFF;


    }
    return 0;
}
