
/*==================[inclusions]=============================================*/
#include "board.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void board_init(void)
{
	/* Activación de clock para los puertos utilizados */
	
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	//SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
	//SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	//SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;

	
	/* =========== LED ROJO =============== */
	
	/* asigna funcionalidad gpio a PTE29 (pag.: 193) */
	PORTE_PCR29 = PORT_PCR_MUX(1);

	/* PTE29 como salida -> led rojo (pag.: 835) */
	GPIOE_PDDR |= (1 << 29);
	
	/* =========== LED VERDE ============== */
	
	/* asigna funcionalidad gpio a PTD5 */
	//PORTD_PCR5 = PORT_PCR_MUX(1);

	/* PTD5 como salida -> led verde */
	//GPIOD_PDDR |= (1 << 5);
	
	/* =========== SW1 =================== */
	
	/* asigna funcionalidad gpio a PTC3 */
	//PORTC_PCR3 = PORT_PCR_MUX(1);

	/* PTC3 como entrada -> SW1 */
	//GPIOC_PDDR &= ~(1 << 3);
	
	/* activación de pull up */
	//PORTC_PCR3 |= (1 << PORT_PCR_PE_SHIFT) | (1 << PORT_PCR_PS_SHIFT);
	
	/* =========== SW3 =================== */
		
	/* asigna funcionalidad gpio a PTC12 */
	//PORTC_PCR12 = PORT_PCR_MUX(1);

	/* PTC3 como entrada -> SW3 */
	//GPIOC_PDDR &= ~(1 << 12);
	
	/* activación de pull up */
	//PORTC_PCR12 |= (1 << PORT_PCR_PE_SHIFT) | (1 << PORT_PCR_PS_SHIFT);


	/* =========== configuracion del adc =================== */
	//ADC0_CFG1 |= 0x71; // registro de configuracion del adc


}

/*int8_t pulsadorSw1_get(void)
{
	return (GPIOC_PDIR & (1 << 3))?0:1;
}

int8_t pulsadorSw3_get(void)
{
	return (GPIOC_PDIR & (1 << 12))?0:1;
}
*/
/*==================[Funciones para manejar los leds]============================================*/
/*
void led_rojo(int x){
	if (x)
		LED_ROJO_ON;
	else
		LED_ROJO_OFF;
}
void led_verde(int x){
	if (x)
		LED_VERDE_ON;
	else
		LED_VERDE_OFF;
}
void led_amarillo(int x){
	return; //no hay led amarillo
}
*/
/*==================[end of file]============================================*/
