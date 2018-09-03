#include "MKL46Z4.h"
#include "board.h"
int main(void)
{
	board_init();
	NVIC_SetPriority(PIT_IRQn, 0);
	NVIC_EnableIRQ(PIT_IRQn);
	init_reloj();
	init_pit();
    return 0;
}
void init_reloj(){
	/*esto que está aca no es algo que esté relacionado al timer, es una cuestion de orden
	 * esto tiene que ver con el reloj del microcontrolador*/
	//habilito el MCGIRCLK (clock interno que voy a usar para el TPM): MCG_C1
	MCG_C1|= 1<<6;
	//selecciono el reloj de 32 KHz para el MCGIRCLK:IRCS de MCG_C2
	MCG_C2 |= 1<<0;
	// divido la frecuencia de entrada, la opcion 3 es dividido 8
	MCG_SC |= MCG_SC_FCRDIV(3);
	// divido el core/system clock por 1 y el bus/flash clock por 2
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
}
