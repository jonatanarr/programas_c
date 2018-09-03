/* includes */
#include "board.h"

void init_systick();
void systick_on();
void systick_off();
void ADC0_init();
void PWM_init();
void led_verde_on();
void led_verde_off();

/* DECLARACION DE VARIABLES */


_Bool E1,E2,E3,E4,E5,E6,E7,E8,E9; 		/*ESTADOS*/
_Bool T1,T2,T3,T4,T5,T6,T7,T8,T9,T10;	/*TRANSICIONES*/
_Bool P1,P2,V,FT=0,CT=0,fl=0;			/*ENTRADAS Y SEÑALES DE TIMER*/


int main(void)
{
	/* INICIALIZACION DE GPIO */
	board_init();
	init_systick();
	ADC0_init();
	PWM_init();



	/* MARCADO INICIAL */
	E1 = 1;
	E2 = 0;
	E3 = 0;
	E4 = 0;
	E5 = 0;
	E6 = 0;
	E7 = 0;
	E8 = 0;
	E9 = 0;

	for (;;) {
		/* SE ADQUIEREN LAS ENTRADAS */

		P1 = pulsadorSw1_get();
		P2 = pulsadorSw3_get();
		FT = fl;

		/* SE DETERMINA EL ESTADO DE LAS TRANSICIONES */

		T1 = E1 && P1 && !P2;
		T2 = E2 && FT;
		T3 = E3 && !FT;
		T4 = E4 && P2;
		T5 = E5 && FT;
		T6 = E6 && !FT;
		T7 = E1 && P2;
		T8 = E7 && FT;
		T9 = E8 && !FT;
		T10 = E9 && P1;


		/* EVOLUCION DE LA RED SEGUN LAS TRANSICIONES */

		if (T1) 	{E1 = 0; E2 = 1;}
		if (T2) 	{E2 = 0; E3 = 1;}
		if (T3) 	{E3 = 0; E4 = 1;}
		if (T4)		{E4 = 0; E5 = 1;}
		if (T5) 	{E5 = 0; E6 = 1;}
		if (T6) 	{E6 = 0; E1 = 1;}
		if (T7) 	{E1 = 0; E7 = 1;}
		if (T8) 	{E7 = 0; E8 = 1;}
		if (T9) 	{E8 = 0; E9 = 1;}
		if (T10) 	{E9 = 0; E5 = 1;}


		/* SETEO DE LAS SALIDAS */

		V = E3 | E4 | E5 | E8 |E9;
		CT = E2 | E5 | E7;



		if (CT == 1){systick_on();}
		else{systick_off();}

		if (V==1){led_verde_on();}
		else{led_verde_off();}

    }
    return 0;
}





void init_systick(){

	/* INICIALIZO EL SYSTICK */

	NVIC_SetPriority( SysTick_IRQn, 0); // Seteo de la prioridad de la interrupcion del systick
	NVIC_EnableIRQ( SysTick_IRQn);		// habilita la interrupcion del systick en el micro

	SysTick->LOAD=6553600; 				// contaremos 6553600 pulsos para obtener cinco segundos.
	SysTick->VAL=0; 					// limpio el contador

}



void SysTick_Handler()
{
	SysTick->CTRL; 						// limpio la bandera de interrupcion
	fl = 1;

}

void systick_on()
{
	SysTick->CTRL=3;					// clock alterno:core(32768*640)/16, interrupcion habilitada, contador habilitado
}

void systick_off()
{
	SysTick->CTRL &= ~1;				// desactivo el systick
	SysTick->VAL=0; 					// limpio el contador
	fl = 0;
}


void PWM_init(){

	SIM_SCGC5	|= SIM_SCGC5_PORTD_MASK;	// dar clock al modulo del port D (led verde)
	PORTD_PCR5	|= PORT_PCR_MUX(4);  		// se setea funcionalidad TPM0_CH5
	SIM_SCGC6	|= SIM_SCGC6_TPM0_MASK;  	// clock al modulo TPM0
	SIM_SOPT2	|= SIM_SOPT2_TPMSRC(1);		// selecciono el clock (se usa el por defecto)
	TPM0_SC 	|= TPM_SC_PS(0) ; 			// selecciono factor de division de prescaler, divido por 1
	TPM0_MOD	= 255;  					// seteo el periodo del PWM
	TPM0_C5V	= 0;  						// seteo el ciclo de trabajo inicial para led verde
	TPM0_C5SC 	|= 0x2c; 					// PWM al CH5 led verde
	TPM0_CNT	= TPM_CNT_COUNT(0) ;  		// se setea el contador del PWM en cero
	TPM0_SC		|= TPM_SC_CMOD(1); 			// arranca a contar PWM
}

void ADC0_init(){
	SIM_SCGC6	|= SIM_SCGC6_ADC0_MASK;		// clock al modulo ADC0
	ADC0_CFG1	|= 0x10;  					// se configura: resolucion 8-bits, consumo normal, tiempo de muestreo largo
	ADC0_SC3	|= ADC_SC3_ADCO_MASK | ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(0);   // se configura conversion continua,Habilita la función promedio del hardware del ADC, se pomedian 4 muestras
	ADC0_SC1A	= 0x03;						// se selecciona el canal 0 DP3 del light sensor y comienza a convertir
}

void led_verde_on(){
	TPM0_C5V=ADC0_RA;						// se asigna el valor convertido del ADC0 al TPM0 (valor de match)
}

void led_verde_off(){
	TPM0_C5V=0;								// cuando quiero apagar el led, asigno 0 al TPM0
}


