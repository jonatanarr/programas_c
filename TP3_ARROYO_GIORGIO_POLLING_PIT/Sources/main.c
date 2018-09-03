/* includes */
#include "board.h"
/* DECLARACION DE VARIABLES */

_Bool E1,E2,E3,E4,E5,E6,E7,E8,E9; /*ESTADOS*/
_Bool T1,T2,T3,T4,T5,T6,T7,T8,T9,T10; /*TRANSICIONES*/
_Bool P1,P2,CT,FT,V;/* ENTRADAS Y SEÑALES DE TIMER*/


void init_reloj();
void init_pit();

int main(void)
{

	/* INICIALIZACION DE GPIO */
	board_init();
	NVIC_SetPriority(PIT_IRQn, 0);
	NVIC_EnableIRQ(PIT_IRQn);


	/* MARCADO INICIAL */
	E1 = 1;
	E2 = 0;
	E3 = 0;
	E4 = 0;
	E5 = 0;
	E6 = 0;
	E7 = 0;
	E8 = 0;
	E9 = 0;

	for (;;) {
		/* SE ADQUIEREN LAS ENTRADAS */
		P1 = pulsadorSw1_get();
		P2 = pulsadorSw3_get();

		/* SE DETERMINA EL ESTADO DE LAS TRANSICIONES */
		T1 = E1 && P1 && !P2;
		T2 = E2 && FT;
		T3 = E3 && !FT;
		T4 = E4 && P2;
		T5 = E5 && FT;
		T6 = E6 && !FT;
		T7 = E1 && P2;
		T8 = E7 && FT;
		T9 = E8 && !FT;
		T10 = E9 && P1;


		/* EVOLUCION DE LA RED SEGUN LAS TRANSICIONES */
		if (T1) {E1 = 0; E2 = 1;}
		if (T2) {E2 = 0; E3 = 1;}
		if (T3) {E3 = 0; E4 = 1;}
		if (T4) {E4 = 0; E5 = 1;}
		if (T5) {E5 = 0; E6 = 1;}
		if (T6) {E6 = 0; E1 = 1;}
		if (T7) {E1 = 0; E7 = 1;}
		if (T8) {E7 = 0; E8 = 1;}
		if (T9) {E8 = 0; E9 = 1;}
		if (T10) {E9 = 0; E5 = 1;}


		/* SETEO DE LAS SALIDAS */

		V = E3 | E4 | E5 | E8 |E9;
		CT = E2 | E5 |E7;


		if (V==1){led_verde(1);}
		else led_verde(0);

		if (CT == 1)
		{
			init_reloj();
			init_pit();
		}
		else {FT = 0;}

    }
    return 0;
}

void init_reloj(){
	/*esto que está aca no es algo que esté relacionado al timer, es una cuestion de orden
	 * esto tiene que ver con el reloj del microcontrolador*/

	//habilito el MCGIRCLK (clock interno que voy a usar para el TPM): MCG_C1
	MCG_C1|= 1<<6;
	//selecciono el reloj de 4 MhZ para el MCGIRCLK:IRCS de MCG_C2
	MCG_C2 |= 1<<0;
	// divido la frecuencia de entrada, la opcion 3 es dividido 8 - 500000 Hz
	MCG_SC |= MCG_SC_FCRDIV(3);
	// divido el core/system clock por 1 (500000 Hz) y el bus/flash clock por 2 (250000 Hz)
	SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(1);
}


void init_pit(){
	//dar clock al modulo PIT
	SIM_SCGC6|=(1<<23);
	// enciendo el PIT
    PIT_MCR = 0x00;
    // SELECCIONO Timer 0
    PIT_LDVAL0 = 1249999; // INICIO EL TIMER EN ESA CUENTA PARA OBTENER 5SEGS
    PIT_TCTRL0 |= 0x3; // enable Timer 0 interrupts y activo el timer 0
}

void PIT_IRQHandler(){

	//limpio la flag
	PIT_TFLG0 |= (1<<0);
	LED_ROJO_TOGGLE;
	FT = 1;
    PIT_MCR = (1<<1);
    PIT_TCTRL0 = 0x1;

}
