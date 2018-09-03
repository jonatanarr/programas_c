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
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"

#include "fsl_sim_hal.h"
#include "fsl_mcg_hal.h"
#include "fsl_tpm_hal.h"
#include "fsl_pit_hal.h"
#include "fsl_smc_hal.h"
#include "fsl_port_hal.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "key.h"
#include "board.h"
#include "MKL46Z4.h"
#include "mma8451.h"
/*==================[macros and definitions]=================================*/
typedef enum
{
	EST_MODO_BAJO_CONSUMO = 0,
	EST_MODO_RUN,
}estMefControlConsumo_enum;

typedef enum
{
	EST_LED_ROJO_INIT = 0,
	EST_LED_ROJO_APAGADO,
	EST_LED_ROJO_TITILANDO,
}estMefControlLed_enum;

/*==================[internal data declaration]==============================*/



static int32_t timLed;



/*==================[internal functions declaration]=========================*/

/*----------------CLOCK----------------*/
/* Configuration for enter VLPR mode. Core clock = 4MHz. */
const clock_manager_user_config_t clockConfigVlpr =
{
    .mcgConfig =
    {
        .mcg_mode           = kMcgModeBLPI, // Work in BLPI mode.
        .irclkEnable        = true,  		// MCGIRCLK enable.
        .irclkEnableInStop  = false, 		// MCGIRCLK disable in STOP mode.
        .ircs               = kMcgIrcFast, 	// Select IRC4M.
		.fcrdiv             = 1,    		// FCRDIV 0. Divide Factor is 1

        .frdiv   = 0,						// No afecta en este modo
        .drs     = kMcgDcoRangeSelLow,  	// No afecta en este modo
        .dmx32   = kMcgDmx32Default,    	// No afecta en este modo

        .pll0EnableInFllMode = false,  		// No afecta en este modo
        .pll0EnableInStop  = false,  		// No afecta en este modo
        .prdiv0            = 0b00,			// No afecta en este modo
        .vdiv0             = 0b00,			// No afecta en este modo
    },
    .simConfig =
    {
        .pllFllSel = kClockPllFllSelPll,	// No afecta en este modo
        .er32kSrc  = kClockEr32kSrcLpo,     // ERCLK32K selection, use LPO.
        .outdiv1   = 0b0000,				// tener cuidado con frecuencias máximas de este modo
        .outdiv4   = 0b101,					// tener cuidado con frecuencias máximas de este modo
    },
    .oscerConfig =
    {
        .enable       = true,	  			// OSCERCLK enable.
        .enableInStop = true, 				// OSCERCLK enable in STOP mode.
    }
};

/* Configuration for enter RUN mode. Core clock = 48MHz. */
const clock_manager_user_config_t clockConfigRun =
{
    .mcgConfig =
    {
        .mcg_mode           = kMcgModePEE,  // Work in PEE mode.
        .irclkEnable        = true,  		// MCGIRCLK enable.
        .irclkEnableInStop  = false, 		// MCGIRCLK disable in STOP mode.
        .ircs               = kMcgIrcFast, 	// Select IRC4M.
        .fcrdiv             = 0,    		// FCRDIV 0. Divide Factor is 1

        .frdiv   = 4,						// Divide Factor is 4 (128) (de todas maneras no afecta porque no usamos FLL)
        .drs     = kMcgDcoRangeSelMid,  	// mid frequency range (idem anterior)
        .dmx32   = kMcgDmx32Default,    	// DCO has a default range of 25% (idem anterior)

        .pll0EnableInFllMode = true,  		// PLL0 enable in FLL mode
        .pll0EnableInStop  = true,  		// PLL0 enable in STOP mode
        .prdiv0            = 0b11,			// divide factor 4 (Cristal 8Mhz / 4 * 24)
        .vdiv0             = 0b00,			// multiply factor 24
    },
    .simConfig =
    {
        .pllFllSel = kClockPllFllSelPll,    // PLLFLLSEL select PLL.
        .er32kSrc  = kClockEr32kSrcLpo,     // ERCLK32K selection, use LPO.
        .outdiv1   = 0b0000,				// Divide-by-1.
        .outdiv4   = 0b001,					// Divide-by-2.
    },
    .oscerConfig =
    {
        .enable       = true,  				// OSCERCLK enable.
        .enableInStop = true, 				// OSCERCLK enable in STOP mode.
    }
};
/* Initialize clock. */
void ClockInit(void)
{
    /* Set allowed power mode, allow all. */
    SMC_HAL_SetProtection(SMC, kAllowPowerModeAll);

    InitOsc0();
    //Arranca en VLPR
    CLOCK_SYS_SetConfiguration(&clockConfigVlpr);
}

/*----------------PIT----------------*/

void PIT_Init(void)
{
    uint32_t frecPerif;

    SIM_HAL_EnableClock(SIM, kSimClockGatePit0);
    PIT_HAL_Enable(PIT);
    CLOCK_SYS_GetFreq(kBusClock, &frecPerif);
    PIT_HAL_SetTimerPeriodByCount(PIT, 1, frecPerif/10);
    PIT_HAL_SetIntCmd(PIT, 1, true);
    PIT_HAL_SetTimerRunInDebugCmd(PIT, false);
    PIT_HAL_StartTimer(PIT, 1);
    NVIC_ClearPendingIRQ(PIT_IRQn);
    NVIC_EnableIRQ(PIT_IRQn);
}

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

int main(void)
{
	//int16_t acc;
	uint32_t i;

    // Enable clock for PORTs, setup board clock source, config pin
    board_init();

   // mma8451_setDataRate(DR_100hz);
    while (1)
    {
		//i = 300000;
		//while (i--);


		//mefControlConsumo();
		//mefControlLed();
		//board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_TOGGLE);




		/* ---------------- MEF QUE CONTROLA EL ACELEROMETRO ---------------- */
   	/*if(mma8451_getFfall() == 1)
    	{
        	   board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_ON);
    	}
        acc = mma8451_getAcX();

    	if (acc > 50)
    			board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_ON);
    	else
    	   board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);

    	if (acc < -50)
    	    board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_ON);
    	else
    	    board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);*/
   }
}

/*==================[end of file]============================================*/


void mefControlConsumo(void)
{
	static estMefControlConsumo_enum estMefControlConsumo = EST_MODO_BAJO_CONSUMO;

	switch (estMefControlConsumo)
	{
		case EST_MODO_BAJO_CONSUMO:
			if(key_getPressEv_Consumo(BOARD_SW_ID_1))
			{
				CLOCK_SYS_SetConfiguration(&clockConfigRun);
				estMefControlConsumo = EST_MODO_RUN;
			}
			break;
		case EST_MODO_RUN:
			if(key_getPressEv_Consumo(BOARD_SW_ID_3))
			{
				CLOCK_SYS_SetConfiguration(&clockConfigVlpr);
				estMefControlConsumo = EST_MODO_BAJO_CONSUMO;
			}
			break;
		default:
			estMefControlConsumo = EST_MODO_BAJO_CONSUMO;
			break;
	}
}

void mefControlLed(void)
{
	static estMefControlLed_enum estMefControlLed = EST_LED_ROJO_INIT;

	switch (estMefControlLed)
	{
		case EST_LED_ROJO_INIT:
			board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_OFF);
			board_setLed(BOARD_LED_ID_VERDE,BOARD_LED_MSG_OFF);

			estMefControlLed = EST_LED_ROJO_APAGADO;
			break;
		case EST_LED_ROJO_APAGADO:
			if(timLed == 0)
			{
				board_setLed(BOARD_LED_ID_VERDE,BOARD_LED_MSG_TOGGLE);
				timLed = 10;
				estMefControlLed = EST_LED_ROJO_APAGADO;
			}
			if(key_getPressEv_Led(BOARD_SW_ID_1))
			{
				board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_ON);
				timLed = 1000;
				estMefControlLed = EST_LED_ROJO_TITILANDO;
			}
			break;
		case EST_LED_ROJO_TITILANDO:
			if(timLed == 0)
			{
				board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_TOGGLE);
				timLed = 1000;
				estMefControlLed = EST_LED_ROJO_TITILANDO;
			}
			if(key_getPressEv_Led(BOARD_SW_ID_3))
			{
				board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_OFF);
				estMefControlLed = EST_LED_ROJO_APAGADO;
			}
			break;
		default:
			estMefControlLed = EST_LED_ROJO_APAGADO;
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













