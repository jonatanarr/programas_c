#include "MKL46Z4.h"

/* joni */
void main(void){

	NVIC_SetPriority(TPM0_IRQn,0);
	NVIC_EnableIRQ(TPM0_IRQn);
	init_reloj();
	init_tpm();


}


void init_reloj(){
	/*esto que est� aca no es algo que est� relacionado al timer, es una cuestion de orden
	 * esto tiene que ver con el reloj del microcontrolador*/

	//habilito el MCGIRCLK (clock interno que voy a usar para el TPM): MCG_C1
	MCG_C1|= 0x2;
	//selecciono el reloj de 32 KHz para el MCGIRCLK:IRCS de MCG_C2
	//no hace falta hacer nada porque est� seleccionado
	//selecciono el MCGIRCLK para el modulo TPM: SIM_SOPT2
	SIM_SOPT2|=(0x03<<24);//se ponen en 1 el pin 24 y 25 completando con ceros

	/* si el numero es sin signo, el desplazamiento completa con ceros, si es con signo, completa con el bit de signo
	 *
	 * con estas instrucciones seleccionamos la fuente de clock del TPM en 32Khz*/
}

void init_tpm(){
	//dar clock al modulo TMP0
	SIM_SCGC6|=(1<<24);
	// dar clk al modulo del port E (led rojo)
	SIM_SCGC5|=(1<<13);
	// dar clk al modulo del port D (led verde)
	SIM_SCGC5|=(1<<12);
	// seleccionar la funcion de los pines para TPM:PORTxPCRn
	PORTD_PCR5|= PORT_PCR_MUX(4);	//verde - pagina 173 estan las alternativas de mux
	PORTE_PCR29|= PORT_PCR_MUX(3); //rojo
	//seteo el prescaler: TPM0_SC ->PS
	//lo dejo en cero (divide por 1)
	//programo los canales 2  y 5 como output compare para hacer TOGGLE de la salida: TPM0_CxSC
	TPM0_C5SC|=84;	//verde pag 569
	TPM0_C2SC|=0x14;	//rojo
	//programo los valores de cuentas para el toggle: TPM0_CnV
	TPM0_C5V=16383;	//verde pag 571
	TPM0_C2V=32767; //rojo
	//programo el valor del modulo (para que vuelva a cero cuenado corresponde): TPM0_MOD
	TPM0_MOD=32767; // pag 568
	// seleccion en el TPM0 el reloj habilitado: TPM0_SC ->CMOD, con lo que el timer arranca
	TPM0_SC|=(1<<3);

}


void TPM0_IRQHandler()
{

	if(TPM0_C5V == 16383)
	{
	TPM0_C5V = 32767;
	} // led verde
	else{
	TPM0_C5V = 16383;
	}
	// limpiar la flag

	TPM0_C5SC |= (1<<7);

}
