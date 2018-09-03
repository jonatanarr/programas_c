
#include "board.h"

#define RETARDO 	100000

enum estados {NADA, VERDE, ROJO}; // estos son los estados segun el diagrama

int main(void)
{
	board_init();
	enum estados estado_actual=NADA;
	enum estados proximo_estado=NADA;
	int8_t fin_tiempo;
	volatile int32_t temporizando;
	fin_tiempo=0;
	temporizando=0;
	for (;;) {
		/* ========== tarea 1: implementa un mecanismo de retardo muy basico e impreciso, sin timers
		 * =========== solo para su uso en la tarea 2 */
		if (temporizando>0){
			temporizando--;
			fin_tiempo=0;
		}
		else
			fin_tiempo=1;
		//nada mas

		// =========== tarea 2: tarea implmentada mediante una MEF con switchs
		switch (estado_actual){// switch de estados
			case NADA:
				LED_ROJO_OFF;			//todo apagado en este estado
				LED_VERDE_OFF;
				//hay pocas señales: uso if/else en vez de switch
				if (pulsadorSw1_get()) //pulsador apretado
					proximo_estado=VERDE;
				break;
			case VERDE:
				LED_ROJO_OFF; 			//el rojo permanece apagado en este estado
				LED_VERDE_ON;           //el verde permanece encendido en este estado
				if (!pulsadorSw1_get())	//veo estado pulsador (con prioridad frente al tiempo)
					proximo_estado=NADA;
				else if (fin_tiempo)			//veo si termino el tiempo
					proximo_estado=ROJO;
				break;
			case ROJO:
				LED_ROJO_ON; 			//el rojo permanece encendido en este estado
				LED_VERDE_OFF;			//el verde permanece apagado en este estado
				if (!pulsadorSw1_get()) 	//veo estado pulsador  (con prioridad frente al tiempo)
					proximo_estado=NADA;
				else if (fin_tiempo)			//veo si termino el tiempo
					proximo_estado=VERDE;
				break;
		}	//fin del switch de estados
		if (estado_actual != proximo_estado){
			if (proximo_estado != NADA)	//cada vez que entro a VERDE o ROJO inicio el temporizador
				temporizando=RETARDO;
			//finalmente, cambio estado
			estado_actual=proximo_estado;
		}
	}	//end for
	return 0;
}	//end main
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
