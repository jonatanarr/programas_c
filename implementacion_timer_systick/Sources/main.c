#include "MKL46Z4.h"
#include "board.h"


int main(void)
{
	board_init();

	// Inicializo y lanzo el systick
	SysTick->LOAD=6553600; // contaremos estos pulsos para obtener un segundo.
	SysTick->VAL=0; // limpio el contador
	SysTick->CTRL=3; // clock alterno:core(32768*640)/16, interrupcion habilitada, contador habilitado

    for (;;) {
    }
    return 0;
}

void SysTick_Handler()
{
	SysTick->CTRL; // limpio la bandera de interrupcion.
	LED_ROJO_TOGGLE;
	LED_VERDE_TOGGLE;
}
