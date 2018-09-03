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
#include "board.h"
#include "MKL46Z4.h"
#include "mma8451.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// SDK Included Files
#include "fsl_sim_hal.h"
#include "fsl_lpsci_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_pit_hal.h"

#include "key.h"


/*==================[macros and definitions]=================================*/

typedef enum
{
	EST_MEF_LED_INIT = 0,
	EST_MEF_LED_ACC,
	EST_MEF_LED_TOGGLE,
}estMefControlLed_enum;

typedef enum
{
	EST_MEF_EJES_INIT = 0,
	EST_MEF_EJEX,
	EST_MEF_EJEY,
	EST_MEF_EJEZ,
}estMefControlAcc_enum;

/*==================[internal data declaration]==============================*/
static int8_t EjeSelect;
static int8_t LedVerde;
static int8_t LedRojo;
static int16_t timLed;
static int16_t acc;

/*==================[internal functions declaration]=========================*/
void mefControlAcc(void);
void mefControlLed(void);
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

/*==================[external functions definition]==========================*/

int main(void)
{



    // Enable clock for PORTs, setup board clock source, config pin
    board_init();
	// Inicializa keyboard
	key_init();
	// Se inicializa el timer PIT
	PIT_Init();

    mma8451_setDataRate(DR_12p5hz);

    while (1)
    {



    	mefControlLed();
    	mefControlAcc();
    }
}

void mefControlAcc(void)
{
	static estMefControlAcc_enum estMefControlAcc = EST_MEF_EJEX;

	switch (estMefControlAcc)
	{
	case EST_MEF_EJEX:
        acc = mma8451_getAcX();

    	if (acc > 50)
    	{
    		LedRojo = 1;
    	}
    	else
    	{
    		LedRojo = 0;
    	}

    	if (acc < -50)
    	{
    		LedVerde = 1;
    	}
    	else
    	{
    		LedVerde = 0;
    	}

		if(EjeSelect == 2)
		{
			estMefControlAcc = EST_MEF_EJEY;
		}
		break;
	case EST_MEF_EJEY:
        acc = mma8451_getAcY();

    	if (acc > 50)
    	{
    		LedRojo = 1;
    	}
    	else
    	{
    		LedRojo = 0;
    	}

    	if (acc < -50)
    	{
    		LedVerde = 1;
    	}
    	else
    	{
    		LedVerde = 0;
    	}

		if(EjeSelect == 3)
		{
			estMefControlAcc = EST_MEF_EJEZ;
		}
		break;
	case EST_MEF_EJEZ:
        acc = mma8451_getAcZ();

    	if (acc > 50)
    	{
    		LedRojo = 1;
    	}
    	else
    	{
    		LedRojo = 0;
    	}

    	if (acc < -50)
    	{
    		LedVerde = 1;
    	}
    	else
    	{
    		LedVerde = 0;
    	}

		if(EjeSelect == 1)
		{
			estMefControlAcc = EST_MEF_EJEX;
		}
		break;
	default:
		estMefControlAcc = EST_MEF_EJEX;
		break;
	}
}


void mefControlLed(void)
{
	static estMefControlLed_enum estMefControlLed = EST_MEF_LED_INIT;
	static int EjeAcc;
	static int LedToggle;

	switch (estMefControlLed)
	{
		case EST_MEF_LED_INIT:
            board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);
            board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);
            EjeAcc = 1;
        	EjeSelect = EjeAcc;
            estMefControlLed = EST_MEF_LED_ACC;
            break;
		case EST_MEF_LED_ACC:
            if (LedVerde == 1)
            {
            	board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_ON);
            }
            else
            {
            	board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);
            }
            if (LedRojo == 1)
            {
                board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_ON);
            }
            else
            {
                board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);
            }
            if (key_getPressEv(BOARD_SW_ID_1) == 1)
            {
                board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);
            	board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);
            	EjeAcc ++;
            	if(EjeAcc > 3)	{EjeAcc = 1;}
            	LedToggle = 2*EjeAcc;
            	timLed = 500;
            	estMefControlLed = EST_MEF_LED_TOGGLE;
            }
            break;
		case EST_MEF_LED_TOGGLE:
            if ((timLed == 0) && (LedToggle != 0))
            {
                board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_TOGGLE);
            	board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_TOGGLE);
            	LedToggle --;
            	timLed = 500;
            	estMefControlLed = EST_MEF_LED_TOGGLE;
            }
            if (LedToggle == 0)
            {
                board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);
            	board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);
            	EjeSelect = EjeAcc;
            	estMefControlLed = EST_MEF_LED_ACC;
            }
            break;
		default:
        	estMefControlLed = EST_MEF_LED_ACC;
        	break;

	}
}
void PIT_IRQHandler(void)
{
    PIT_HAL_ClearIntFlag(PIT, 1);

    key_periodicTask1ms();
    if(timLed)
    {
    	timLed--;
    }
}

/*==================[end of file]============================================*/
















