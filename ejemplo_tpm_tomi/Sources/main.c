#include "MKL46Z4.h"


void main(void)
{
	NVIC_SetPriority(TPM0_IRQn, 0);
	NVIC_EnableIRQ(TPM0_IRQn);
	init_reloj();
	init_tpm();

}

void init_reloj(){

	//habilito el MCGIRCLK (clock interno que voy a usar para el TPM): MCG_C1
	MCG_C1|=0x2;
	//selecciono el reloj de 32 KHz para el MCGIRCLK:IRCS de MCG_C2
	//no hace falta hacer nada porque está seleccionado
	//selecciono el MCGIRCLK para el modulo TPM: SIM_SOPT2
	SIM_SOPT2|=(0x03<<24);
}

void init_tpm(){
	//dar clock al modulo TMP0
	SIM_SCGC6|=(1<<24);
	// dar clk al modulo del port E (led rojo)
	SIM_SCGC5|=(1<<13);
	// dar clk al modulo del port D (led verde)
	SIM_SCGC5|=(1<<12);
	// seleccionar la funcion de los pines para TPM:PORTxPCRn
	PORTD_PCR5|= PORT_PCR_MUX(4);	//verde   Pag 173 en adelante
	PORTE_PCR29|= PORT_PCR_MUX(3); //rojo
	//seteo el prescaler: TPM0_SC ->PS
	//lo dejo en cero (divide por 1)
	//programo los canales 2  y 5 como output compare para hacer TOGGLE de la salida: TPM0_CxSC
	TPM0_C5SC|=84;	//verde  pag 569
	TPM0_C2SC|=0x14;	//rojo
	//programo los valores de cuentas para el toggle: TPM0_CnV
	TPM0_C5V=16383;	//verde
	TPM0_C2V=32767; //rojo
	//programo el valor del modulo (para que vuelva a cero cuenado corresponde): TPM0_MOD
	TPM0_MOD=32767;
	// seleccion en el TPM0 el reloj habilitado: TPM0_SC ->CMOD, con lo que el timer arranca
	TPM0_SC|=(1<<3);
}

void TPM0_IRQHandler()
{
	if(TPM0_C5V==16383)
	{
		TPM0_C5V=32767;
	}
	else
	{
		TPM0_C5V=16383;
	}
	TPM0_C5SC |= (1<<7);
}
