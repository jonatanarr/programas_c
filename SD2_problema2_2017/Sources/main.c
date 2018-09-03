/* Copyright 2017, DSI FCEIA UNR - Sistemas Digitales 2
 *    DSI: http://www.dsi.fceia.unr.edu.ar/
 * Copyright 2017, Diego Alegrechi
 * Copyright 2017, Gustavo Muro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*==================[inclusions]=============================================*/

// Standard C Included Files

int cambio=0,contador=0;

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// SDK Included Files
#include "fsl_sim_hal.h"
#include "fsl_lpsci_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_pit_hal.h"

// Project Included Files
#include "board.h"
#include "key.h"

/*==================[macros and definitions]=================================*/
typedef enum
{
	EST_MEF_LED_INICIALIZACION = 0,
	EST_MEF_LED_ESPERANDO_EV,
}estMefLed_enum;



/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/
void PIT_Init(void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

void PIT_Init(void)
{
    uint32_t frecPerif;

    SIM_HAL_EnableClock(SIM, kSimClockGatePit0);
    PIT_HAL_Enable(PIT);
    CLOCK_SYS_GetFreq(kBusClock, &frecPerif);
    PIT_HAL_SetTimerPeriodByCount(PIT, 1, frecPerif/1000);
    PIT_HAL_SetIntCmd(PIT, 1, true);
    PIT_HAL_SetTimerRunInDebugCmd(PIT, false);
    PIT_HAL_StartTimer(PIT, 1);
    NVIC_ClearPendingIRQ(PIT_IRQn);
    NVIC_EnableIRQ(PIT_IRQn);
}

void mefLed(void)
{
    static estMefLed_enum estMefLed = EST_MEF_LED_INICIALIZACION;

    switch (estMefLed)
    {

        case EST_MEF_LED_INICIALIZACION:
            board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);
            estMefLed = EST_MEF_LED_ESPERANDO_EV;
            break;

        case EST_MEF_LED_ESPERANDO_EV:
            if (cambio==1)
                board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_TOGGLE);
            cambio=0;
            break;
    }
}




void logica () {

	typedef enum
	{
		espero_pulsado = 0,
		manteniendo,
		est_final
	}pul_enum;

	static pul_enum pul ;

	switch (pul){

	case espero_pulsado:
    if(key_getPressEv(BOARD_SW_ID_1)){
	contador=2000;
	pul=manteniendo;
	cambio=1;
    }

	case manteniendo:
		while(key_getPressEv(BOARD_SW_ID_1)&&contador!=0){
	contador--;
		}
	if (contador==0){
		contador=300;
		pul=est_final;

	}
	if(!key_getPressEv(BOARD_SW_ID_1)){
		pul=espero_pulsado;
	}

	case est_final:
	contador--;
		if (contador==0){
			contador=300;
			cambio=1;
		}
	default:
		pul=espero_pulsado;
	}
}


/*==================[external functions definition]==========================*/

int main(void)
{
	// Se inicializan funciones de la placa
	board_init();

	// Inicializa keyboard
	key_init();

	// Se inicializa el timer PIT
	PIT_Init();

    while(1)
    {
    	mefLed();

    }
}

void PIT_IRQHandler(void)
{
    PIT_HAL_ClearIntFlag(PIT, 1);

    key_periodicTask1ms();

    logica ();
}

/*==================[end of file]============================================*/
