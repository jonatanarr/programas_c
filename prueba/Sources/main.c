#include "fsl_device_registers.h"
#include "board.h"

static int i = 0;
#define RETARDO 	100000

enum estados {esperando, esp_desp_p1, llenando, lavando, pausa_lav, vaciando};

int main(void)
{
	int p1=0;
	int tlavado = 10,tllenado = 10, tvaciado = 10;
	enum estados proximo_estado = esperando;
	enum estados estado_actual = esperando;
	//inicio de placa
	board_init();
	int8_t fin_tiempo;
	volatile int32_t temporizando;
	fin_tiempo=0;
	temporizando=0;


    for (;;) {

    if (temporizando>0){
    	temporizando--;
    	fin_tiempo=0;
    }
    else {
    	fin_tiempo=1;
    }

	    lavarropa();


        i++;
    }


	return 0;
}

lavarropa(){
	proximo_estado = estado_actual;
		   switch(estado_actual){
		   case esperando:
			   LED_ROJO_OFF;
			   LED_VERDE_OFF;
			   if(pulsadorSw1_get()){
				   p1=1;
				   proximo_estado=esp_desp_p1;
			   }
			   break;
		   case esp_desp_p1:

			   if((p1==1) & (!pulsadorSw1_get())){
				   LED_ROJO_OFF;
				   LED_VERDE_OFF;
				   proximo_estado=llenando;
			   }
			   if((p1==2) & (!pulsadorSw1_get())){
			   		LED_ROJO_OFF;
			   		LED_VERDE_OFF;
			   		proximo_estado=pausa_lav;
			   }
			   if((p1==3) & (!pulsadorSw1_get())){
				   LED_ROJO_TOGGLE;
				   LED_VERDE_TOGGLE;
				   proximo_estado=lavando;
			   }

			   break;
		   case llenando:
			   LED_ROJO_ON;
			   LED_VERDE_OFF;
			   //temporizando = tllenado;
			   if(fin_tiempo){
				   proximo_estado=lavando;
			   }
			   break;
		   case lavando:
			   LED_ROJO_ON;
			   LED_VERDE_ON;
			   //temporizando = tlavado;
			   if(!fin_tiempo & pulsadorSw1_get()){
				   tlavado = temporizando;
				   p1=2;
				   proximo_estado=esp_desp_p1;
			   }
			   if(fin_tiempo){
				   proximo_estado=vaciando;
			   }
			   break;
		   case pausa_lav:
			   LED_ROJO_TOGGLE;
			   LED_VERDE_TOGGLE;
		   	   if(pulsadorSw1_get()){
		   		   p1=3;
		   		   proximo_estado=esp_desp_p1;
		   	   }
		   	   if(pulsadorSw3_get()){
		   		   proximo_estado=vaciando;
		   	   }
		   	   break;

		   case vaciando:
			   LED_ROJO_OFF;
			   LED_VERDE_ON;
			   temporizando = tvaciado;
			   if(fin_tiempo){
				   	  proximo_estado=esperando;
			   }
			   break;
		   }

		   if (estado_actual != proximo_estado){
		   			if (proximo_estado != esperando)	//cada vez que entro a VERDE o ROJO inicio el temporizador
		   				temporizando=RETARDO;
		   			//finalmente, cambio estado
		   			estado_actual=proximo_estado;
		   		}

}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
