
#include "fsl_device_registers.h"
#include "board.h"

_Bool E1,E2,E3,E4,E5,R,V,ENT1,ENT2;
_Bool T1,T2,T3,T4;


int main(void)
{
	board_init();
		led_rojo(1);
		led_verde(0);

	    /* Marcado inicial*/

		E1=1;
		E2=1;
		E3=1;
		E4=0;
		E5=0;

    for (;;) {
    	ENT1=pulsadorSw1_get();
    	ENT2=pulsadorSw3_get();

    	T1= E1 && E2 && ENT1;
    	T2= E4 && (!ENT1 || ENT2) ;
    	T3= E2 && E3 && !ENT1 && ENT2;
    	T4= E5 && !ENT2 && ENT1;

    	if (T1) {E1 = 0; E2 = 0; E4=1;}
    	if (T2) {E4 = 0; E1 = 1; E2=1;}
    	if (T3) {E2 = 0; E3 = 0; E5=1;}
    	if (T4) {E5 = 0; E2 = 1; E3=1;}

    	R= E3;
    	V= (E4 && ENT1) || (E5 && !ENT1);

    	if (R==1){LED_ROJO_ON;}
    			else LED_ROJO_OFF;

    	if (V==1){LED_VERDE_ON;}
    			else LED_VERDE_OFF;
    }

    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
