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
#include "MKL46Z4.h"
#include "mma8451.h"

/*==================[macros and definitions]=================================*/
typedef enum
{
	EST_MEF_ESPERANDO_ACTIVACION = 0,
	EST_MEF_ACTIVACION,
	EST_MEF_PREROBO,
	EST_MEF_ROBO,
} estMefModo_enum;

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/
void PIT_Init(void);

/*==================[internal data definition]===============================*/
static int16_t LedVerdeTime=0;
static int16_t LedVerdeToggle=0;
static int16_t PreRoboTime=0;
static bool inicio;

static int16_t accX;
static int16_t accY;
static int16_t accZ;

static int16_t accXcalib;
static int16_t accYcalib;
static int16_t accZcalib;

/*==================[external data definition]===============================*/

#define RANGE_G						50		// Umbral de movimiento disparo alarma
#define LED_VERDE_TIME_ACTIVACION	5000	// LED verde encendido  durante 5 seg durante activacion de alarma
#define LED_VERDE_TIME_TOGGLE		1000	// LED verde destella cada 1 seg durante alarma activada
#define TIME_BEFORE_SHUT			3000	// Tiempo antes de disparo de alarma luego de detección de movimiento


/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

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

void mefModo(void)
{
	static estMefModo_enum estMefModo = EST_MEF_ESPERANDO_ACTIVACION;

    	switch (estMefModo)
        {
            case EST_MEF_ESPERANDO_ACTIVACION:
            	if (key_getPressEv(BOARD_SW_ID_1))
                {
                    inicio = 1;
            		LedVerdeTime = LED_VERDE_TIME_ACTIVACION;
                    board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_ON);
                }

                if (!LedVerdeTime && inicio)
                {
                	LedVerdeToggle = LED_VERDE_TIME_TOGGLE;
                	board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);

                	accXcalib = abs(mma8451_getAcX());
                	accYcalib = abs(mma8451_getAcY());
                	accZcalib = abs(mma8451_getAcZ());

                    estMefModo = EST_MEF_ACTIVACION;
                }
                break;

            case EST_MEF_ACTIVACION:
                if (!LedVerdeToggle)
                {
                    LedVerdeToggle = LED_VERDE_TIME_TOGGLE;
                	board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_TOGGLE);
                }

            	if (key_getPressEv(BOARD_SW_ID_1))
                {
            		board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);
                    estMefModo = EST_MEF_ESPERANDO_ACTIVACION;
                    inicio = 0;
                }

                accX = abs(mma8451_getAcX());
                accX = abs(accX-accXcalib);

                accY = abs(mma8451_getAcY());
                accY = abs(accY-accYcalib);

                accZ = abs(mma8451_getAcZ());
                accZ = abs(accZ-accZcalib);

                if (accX > RANGE_G  || accY > RANGE_G || accZ > RANGE_G)
                {
                	PreRoboTime = TIME_BEFORE_SHUT;
                	board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);
                	estMefModo = EST_MEF_PREROBO;
                }
                break;

            case EST_MEF_PREROBO:
            	if (PreRoboTime == 0)
            	{
            		 board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_ON);
            		 estMefModo = EST_MEF_ROBO;
            	}
            	if (key_getPressEv(BOARD_SW_ID_1))
            	{
            		board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);
            		estMefModo = EST_MEF_ESPERANDO_ACTIVACION;
            		inicio = 0;
            	}

            	break;

            case EST_MEF_ROBO:
            	if (key_getPressEv(BOARD_SW_ID_1))
            	{
            		board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);
            		estMefModo = EST_MEF_ESPERANDO_ACTIVACION;
            		inicio = 0;
            	}

            	break;

            default:
            	estMefModo = EST_MEF_ESPERANDO_ACTIVACION;

            	break;
        }
}

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
    	mefModo();
    }
}

void PIT_IRQHandler(void)
{
    PIT_HAL_ClearIntFlag(PIT, 1);

    key_periodicTask1ms();

    if (LedVerdeTime)
        LedVerdeTime--;

    if (LedVerdeToggle)
        LedVerdeToggle--;

    if (PreRoboTime)
    	PreRoboTime--;
}

/*==================[end of file]============================================*/
















