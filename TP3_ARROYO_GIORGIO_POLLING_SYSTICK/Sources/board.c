
/*==================[inclusions]=============================================*/
#include "board.h"

/*==================[external functions definition]==========================*/
void board_init(void)
{
	/* Activación de clock para los puertos utilizados */
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	
	/* =========== SW1 =================== */
	
	/* asigna funcionalidad gpio a PTC3 */
	PORTC_PCR3 = PORT_PCR_MUX(1);

	/* PTC3 como entrada -> SW1 */
	GPIOC_PDDR &= ~(1 << 3);

	/* activación de pull up */
	PORTC_PCR3 |= (1 << PORT_PCR_PE_SHIFT) | (1 << PORT_PCR_PS_SHIFT);
	
	/* =========== SW3 =================== */
		
	/* asigna funcionalidad gpio a PTC12 */
	PORTC_PCR12 = PORT_PCR_MUX(1);

	/* PTC3 como entrada -> SW3 */
	GPIOC_PDDR &= ~(1 << 12);
	
	/* activación de pull up */
	PORTC_PCR12 |= (1 << PORT_PCR_PE_SHIFT) | (1 << PORT_PCR_PS_SHIFT);

}

/*==================[Funciones para manejar los pulsadores]=======================================*/


int8_t pulsadorSw1_get(void)
{
	return (GPIOC_PDIR & (1 << 3))?0:1;
}

int8_t pulsadorSw3_get(void)
{
	return (GPIOC_PDIR & (1 << 12))?0:1;
}



/*==================[end of file]============================================*/
