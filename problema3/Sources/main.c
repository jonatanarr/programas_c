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
	INICIO_SECUENCIA = 0,
	SECUENCIA_1_R,
	SECUENCIA_2_R,
	SECUENCIA_1_V,
	SECUENCIA_2_V,
}estMefDet_Secuencia_enum;

typedef enum
{
	EST_MEF_CONTROL_INICIALIZACION = 0,
	DET_SECUENCIA,
	LEDS_APAGADOS,
	LEDS_TITILANDO,

}estMefDeControl_enum;

typedef enum
{
	EST_MEF_LED_TI_INICIALIZACION = 0,
	EVENTO_TITILANDO,

}estMefDeLedTitilando_enum;


/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/
void PIT_Init(void);

/*==================[internal data definition]===============================*/
static bool led_verde_off;
static bool led_rojo_off;
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

void mefSecuencia(void)
{
    static estMefDet_Secuencia_enum estMefSecuencia = INICIO_SECUENCIA;
    switch (estMefSecuencia)
    {
        case INICIO_SECUENCIA:
            if (key_getPressEv(BOARD_SW_ID_1))
            {
            	estMefSecuencia=SECUENCIA_1_R;
            }
            if (key_getPressEv(BOARD_SW_ID_3))
            {
            	estMefSecuencia=SECUENCIA_1_V;
            }
            break;
        case SECUENCIA_1_R:
            if (key_getPressEv(BOARD_SW_ID_1))
            {
            	estMefSecuencia=SECUENCIA_2_R;
            }
            if (key_getPressEv(BOARD_SW_ID_3))
            {
            	estMefSecuencia=INICIO_SECUENCIA;
            }
            break;
        case SECUENCIA_2_R:
            if (key_getPressEv(BOARD_SW_ID_1))
            {
            	estMefSecuencia=INICIO_SECUENCIA;
            }
            if (key_getPressEv(BOARD_SW_ID_3))
            {
            	estMefSecuencia=INICIO_SECUENCIA;
            	board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);
            	led_rojo_off = 1;
            }
            break;
        case SECUENCIA_1_V:
			if (key_getPressEv(BOARD_SW_ID_1))
			{
				estMefSecuencia=SECUENCIA_2_V;
			}
			if (key_getPressEv(BOARD_SW_ID_3))
			{
				estMefSecuencia=INICIO_SECUENCIA;
			}
			break;
        case SECUENCIA_2_V:
			if (key_getPressEv(BOARD_SW_ID_1))
			{
				estMefSecuencia=INICIO_SECUENCIA;
			}
			if (key_getPressEv(BOARD_SW_ID_3))
			{
				estMefSecuencia=INICIO_SECUENCIA;
				board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);
				led_verde_off = 1;
			}
			break;
    }
}

void mefDeControl(void)
{
    static estMefDeControl_enum estMefDeControl = EST_MEF_CONTROL_INICIALIZACION;
    switch (estMefDeControl)
    {
        case EST_MEF_CONTROL_INICIALIZACION:
            board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_ON);
            board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_ON);
            led_verde_off=0;
            led_rojo_off=0;
            tim=5000;

            estMefDeControl = DET_SECUENCIA;
            break;
        case DET_SECUENCIA:
        	mefSecuencia();
            if (led_verde_off && led_rojo_off)
            {
            	estMefDeControl=LEDS_APAGADOS;
            	tim=5000;
            }
            if ((tim==0) && (!led_verde_off) && (!led_rojo_off))
            {
            	estMefDeControl=LEDS_TITILANDO;
            }
            break;
        case LEDS_APAGADOS:
            if (tim==0)
            {
            	board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_ON);
            	board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_ON);
				led_verde_off=0;
				led_rojo_off=0;
				tim=5000;
            	estMefDeControl=DET_SECUENCIA;
            }
            break;
        case LEDS_TITILANDO:
        	mefLedsTitilando();
            if (key_getPressEv(BOARD_SW_ID_1) || key_getPressEv(BOARD_SW_ID_3))
            {
            	board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_ON);
            	board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_ON);
				led_verde_off=0;
				led_rojo_off=0;
				tim=5000;
            	estMefDeControl=DET_SECUENCIA;
            }
            break;
    }
}

void mefLedsTitilando(void)
{
    static estMefDeLedTitilando_enum estMefDeLedTitilando = EST_MEF_LED_TI_INICIALIZACION;
    switch (estMefDeLedTitilando)
    {
        case EST_MEF_LED_TI_INICIALIZACION:
            board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_ON);
            board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_ON);
            tim=500;

            estMefDeLedTitilando = EVENTO_TITILANDO;
            break;
        case EVENTO_TITILANDO:
            if (tim == 0)
            {
                board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_TOGGLE);
                board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_TOGGLE);
            	estMefDeLedTitilando=EVENTO_TITILANDO;
            	tim=500;
            }
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
    	mefDeControl();
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
