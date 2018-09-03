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
	EST_MEF_MODO_SEC = 0,
	EST_MEF_MODO_BLINKY,
	LED_VERDE_500mS,
	LED_RV_500mS,
}estMefModo_enum;

typedef enum
{
	EST_MEF_LED_INICIALIZACION_SIMULTANEO = 0,
	LED_RV_EV,

}estMefSimult_enum;

typedef enum
{
	EST_MEF_SEC_1 = 0,
	EST_MEF_SEC_2,
	EST_MEF_SEC_3,

}estMefSec_enum;


/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/
void PIT_Init(void);

/*==================[internal data definition]===============================*/
static int32_t tim;
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

void mef_parpadeo_inicial(void)
{
    static estMefParpadeo_enum estMefLed = EST_MEF_LED_INICIALIZACION_PARPADEO;

    switch (estMefLed)
    {
        case EST_MEF_LED_INICIALIZACION_PARPADEO:
            board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);
            board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);
            tim=0;
            estMefLed = LED_ROJO_500mS;
            break;

        case LED_ROJO_500mS:
            if (tim==0)
            {
                board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_ON);
            	board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);
            	tim = 500;
            	estMefLed = LED_VERDE_500mS;
            }
            break;
        case LED_VERDE_500mS:
            if (tim==0)
            {
                board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_ON);
            	board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);
            	tim = 500;
                estMefLed = LED_RV_500mS;
            }
            break;
        case LED_RV_500mS:
            if (tim==0)
            {
                board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_ON);
                board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_ON);
                tim = 500;
                estMefLed = LED_ROJO_500mS;
            }
            break;
    }
}

void mef_parpadeo_simultaneo(void)
{
    static estMefSimult_enum estMefSimult = EST_MEF_LED_INICIALIZACION_SIMULTANEO;

    switch (estMefSimult)
    {
        case EST_MEF_LED_INICIALIZACION_SIMULTANEO:
            board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);
            board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);
            tim=0;
            estMefSimult = LED_RV_EV;
            break;

        case LED_RV_EV:
            if (tim==0)
            {
                board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_TOGGLE);
            	board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_TOGGLE);
            	tim = 500;
            }
            break;
    }
}

void mefSecuencia(void)
{
    static estMefSecuencia_enum estMefSecuencia = SECUENCIA_INICIALIZACION;

    switch (estMefSecuencia)
    {
        case SECUENCIA_INICIALIZACION:
            board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);
            board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);
            tim=0;
            estMefSecuencia = SECUENCIA_PARPADEO_INIC;
            break;

        case SECUENCIA_PARPADEO_INIC:
        	mef_parpadeo_inicial();
            if (key_getPressEv(BOARD_SW_ID_1))
            {
            	estMefSecuencia = PRIMER_PULSADO;
            }
            break;

        case PRIMER_PULSADO:
        	mef_parpadeo_simultaneo();
            if (key_getPressEv(BOARD_SW_ID_1))
            {
                board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_ON);
                board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_ON);
              	estMefSecuencia = SEGUNDO_PULSADO;
            }
            break;
        case SEGUNDO_PULSADO:
            if (key_getPressEv(BOARD_SW_ID_1))
            {
              	estMefSecuencia = SECUENCIA_PARPADEO_INIC;
            }
            break;
        default:
          	estMefSecuencia = SECUENCIA_PARPADEO_INIC;
          	break;
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
    	mefSecuencia();
    }
}

void PIT_IRQHandler(void)
{
    PIT_HAL_ClearIntFlag(PIT, 1);

    key_periodicTask1ms();
    if(tim)
    {
    	tim--;
    }
}

/*==================[end of file]============================================*/
