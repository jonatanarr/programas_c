#include "fsl_device_registers.h"
#include "board.h"
static int i = 0;

int main(void)
{
	board_init();
	led_verde(0);
	led_rojo(0);
	SysTick->LOAD=1310720;
	SysTick->VAL =0;
	SysTick->CTRL=3;
    /* Write your code here */

    /* This for loop should be replaced. By default this loop allows a single stepping. */
    for (;;) {
        i++;
    }
    /* Never leave main */
    return 0;
}
void SysTick_Handler(){

	SysTick->CTRL;
	LED_VERDE_TOGGLE;
	LED_ROJO_TOGGLE;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
