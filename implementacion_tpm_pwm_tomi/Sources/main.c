
#include "MKL46Z4.h"
#include "board.h"
void init_reloj();
void init_tpm();
void main(void)
{
	//NVIC_SetPriority(TPM0_IRQn, 0);
	//NVIC_EnableIRQ(TPM0_IRQn);
	init_reloj();
	init_tpm();
}

void init_reloj()
{
	MCG_C1 |= (1<<6) | (1<<1);
	MCG_C2 |= (0<<0);
	SIM_SOPT2 |= (3<<24);
}

void init_tpm(){
	//dar clock al modulo TMP0
	SIM_SCGC6|=(1<<24);
	// dar clk al modulo del port E (led rojo)
	SIM_SCGC5|=(1<<13);
	// dar clk al modulo del port D (led verde)
	SIM_SCGC5|=(1<<12);
	// seleccionar la funcion de los pines para TPM:PORTxPCRn
	PORTD_PCR5|= PORT_PCR_MUX(4);	//verde CH5   Pag 173 en adelante
	PORTE_PCR29|= PORT_PCR_MUX(3);	//rojo CH2
	TPM0_C5SC |= (1<<5) | (1<<3); // PWM al CH5
	TPM0_C2SC |= (1<<5) | (1<<3); // PWM al CH2
	TPM0_MOD = 32767;
	TPM0_C5V=25000;
	TPM0_C2V=13000;
	TPM0_SC |= (1<<3);
}
