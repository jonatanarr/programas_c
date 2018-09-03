
#include "fsl_device_registers.h"
#include "board.h"

static int i = 0;


_Bool L1, L2, L3, L4, L5, L6;
_Bool T1, T2, T3, T4;
_Bool Ent1, Ent2;
_Bool R_T, R, V;


int main(void){
	//inicialización GPIO
	board_init();
	led_rojo(0);
	led_verde(0);
	//Marcado inicial
	L1 = 1;
	L4 = 1;
	L2 = 0;
	L3 = 0;
	L5 = 0;
	L6 = 0;

	for(;;)	{
		// se adquieren las entradas
		Ent1 = pulsadorSw1_get();
		Ent2 = pulsadorSw3_get();

		//se determina el estado de las transiciones
		T1 = L1 && Ent1;
		T2 = L2 && Ent2 && !Ent1;
		T3 = L3 && L4 && !Ent2 && Ent1;
		T4 = L5 && L6 && Ent1 && Ent2;

		//evolución de la red según las reglas de marcado
		if (T1) {L1=0; L2=1;}
		if (T2) {L2=0; L5=1; L3=1;}
		if (T3) {L3=0; L4=0; L6=1;}
		if (T4) {L5=0; L6=0; L1=1; L4=1;}

		//producción de las salidas (SET)
		R = L1 || ( L3 && Ent2 ) || L5;
		//R_T = L2 || L6;
		V = L4;

		if (R==1){LED_ROJO_ON;}
		else LED_ROJO_OFF;

		if (V==1){led_verde(1);}
		else led_verde(0);
		//if (R_T==1){LED_ROJO_TOGGLE;}
	}

return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
