
/* Copyright 2017, DSI FCEIA UNR - Sistemas Digitales 2
 *    DSI: http://www.dsi.fceia.unr.edu.ar/
 * Copyright 2018, Esteban Almiron
 * Copyright 2018, Diego Alegrechi
 * Copyright 2018, Gustavo Muro
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
#include <string.h>


// SDK Included Files
#include "fsl_sim_hal.h"
#include "fsl_lpsci_hal.h"
#include "fsl_device_registers.h"
#include "fsl_port_hal.h"


/*---------------clock---------------*/
#include "fsl_clock_manager.h"
#include "fsl_mcg_hal.h"
#include "fsl_smc_hal.h"


#include "fsl_pit_hal.h"
#include "fsl_slcd_hal.h"

// Project Included Files
#include "board.h"
#include "key.h"
#include "LCD.h"
#include "MKL46Z4.h"
#include "mma8451.h"
#include "key.h"
#include "math.h"

/*==================[macros and definitions]=================================*/
/* OSC0 configuration. */
#define OSC0_XTAL_FREQ 8000000U

/* EXTAL0 PTA18 */
#define EXTAL0_PORT   PORTA
#define EXTAL0_PIN    18
#define EXTAL0_PINMUX kPortPinDisabled

/* XTAL0 PTA19 */
#define XTAL0_PORT   PORTA
#define XTAL0_PIN    19
#define XTAL0_PINMUX kPortPinDisabled

typedef enum
{
	EST_MEF_INIT = 0,
	EST_MEF_ESPERANDO_CAIDA,
	EST_MEF_DETECTANDO_CHOQUE,
	EST_MEF_CALCULANDO_ACC_MAX,
	EST_MEF_MOSTRAR_ACC_LCD,
}estMefDetCaida_enum;



typedef enum
{
	EST_LED_ROJO_INIT = 0,
	EST_LED_ROJO_APAGADO,
	EST_LED_ROJO_TITILANDO,
}estMefControlLed_enum;

/*==================[internal data declaration]==============================*/
static	int8_t led_titila;
static	int32_t timLed;
static	int32_t timAcc;
static 	int32_t timOff;

static bool ControlRun;
static bool ControlVlpr;






static  uint8_t BufLCD[6];

/*==================[internal functions declaration]=========================*/

void ClockInit(void);
void PIT_Init(void);


void mefDetCaida(void);
void mefControlLed(void);

void vlprMode(void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

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

void InitOsc0(void)
{
    // OSC0 configuration.
    osc_user_config_t osc0Config =
    {
        .freq                = OSC0_XTAL_FREQ,
        .hgo                 = kOscGainLow,
        .range               = kOscRangeVeryHigh,
        .erefs               = kOscSrcOsc,
        .enableCapacitor2p   = false,
        .enableCapacitor4p   = false,
        .enableCapacitor8p   = false,
        .enableCapacitor16p  = false,
    };

    /* Setup board clock source. */
    // Setup OSC0 if used.
    // Configure OSC0 pin mux.
    PORT_HAL_SetMuxMode(EXTAL0_PORT, EXTAL0_PIN, EXTAL0_PINMUX);
    PORT_HAL_SetMuxMode(XTAL0_PORT, XTAL0_PIN, XTAL0_PINMUX);

    CLOCK_SYS_OscInit(0U, &osc0Config);
}
/* Initialize clock. */
void ClockInit(void)
{
    /* Set allowed power mode, allow all. */
    SMC_HAL_SetProtection(SMC, kAllowPowerModeAll);

    InitOsc0();
    //Arranca en VLPR
    CLOCK_SYS_SetConfiguration(&clockConfigRun);
}


/*----------------PIT----------------*/

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


	board_init();

	while(1)
    {
		mefDetCaida();
		mefControlLed();
	}
}

void mefDetCaida(void)
{
	static 	int32_t Acc_X;
	static 	int32_t Acc_Y;
	static	int32_t Acc_Z;
	static 	uint32_t Acc;
	static 	uint32_t Acc_Max;

	static estMefDetCaida_enum estMefDetCaida = EST_MEF_INIT;

	switch (estMefDetCaida)
	{
	case EST_MEF_INIT:
		mma8451_init();
		vfnLCD_All_Segments_OFF();
		timLed = 10;
		board_setLed(BOARD_LED_ID_VERDE,BOARD_LED_MSG_OFF);
		estMefDetCaida = EST_MEF_ESPERANDO_CAIDA;
		CLOCK_SYS_SetConfiguration(&clockConfigVlpr);
		ControlRun = 1;
		break;

	case EST_MEF_ESPERANDO_CAIDA:
		if(mma8451_getFfall())
		{
			mma8451_DataReadyMode();
			board_setLed(BOARD_LED_ID_VERDE,BOARD_LED_MSG_OFF);
			timLed = 100;
			led_titila = 1;
			estMefDetCaida = EST_MEF_DETECTANDO_CHOQUE;
		}
		break;
	case EST_MEF_DETECTANDO_CHOQUE:
		Acc_X = mma8451_getAcX();
		Acc_Y = mma8451_getAcY();
		Acc_Z = mma8451_getAcZ();
		Acc = Acc_X*Acc_X + Acc_Y*Acc_Y + Acc_Z*Acc_Z;
		if(Acc >= 7000)//funciona bien con 6000
		{
			timAcc = 100;
			Acc_Max = Acc;
			estMefDetCaida = EST_MEF_CALCULANDO_ACC_MAX;
		}
		break;

	case EST_MEF_CALCULANDO_ACC_MAX:
		Acc_X = mma8451_getAcX();
		Acc_Y = mma8451_getAcY();
		Acc_Z = mma8451_getAcZ();
		Acc = Acc_X*Acc_X + Acc_Y*Acc_Y + Acc_Z*Acc_Z;

		if(Acc>Acc_Max)
		{
			Acc_Max = Acc;
			timAcc = 100;
		}


		if(timAcc == 0)
		{
    		Acc_Max = sqrt(Acc_Max);
			sprintf((char *)BufLCD,"%4d",(int)Acc_Max);
            vfnLCD_Write_Msg(BufLCD);
            Acc_X	= 0;
			Acc_Y 	= 0;
			Acc_Z 	= 0;
			Acc 	= 0;
            Acc_Max = 0;
            timOff = 10000;
            estMefDetCaida = EST_MEF_MOSTRAR_ACC_LCD;
		}
		break;
	case EST_MEF_MOSTRAR_ACC_LCD:
		if(key_getPressEv(BOARD_SW_ID_1) || (timOff == 0))
		{
			led_titila = 0;// variable de comunicacion con la mef de led
			mma8451_FreeFallMode();
			vfnLCD_All_Segments_OFF();
			estMefDetCaida = EST_MEF_ESPERANDO_CAIDA;
			vlprMode();
		}
		break;
	default:
		estMefDetCaida = EST_MEF_ESPERANDO_CAIDA;
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
			estMefControlLed = EST_LED_ROJO_APAGADO;
			break;
		case EST_LED_ROJO_APAGADO:
			if(led_titila == 1)
			{
				board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_ON);
				timLed = 500;
				estMefControlLed = EST_LED_ROJO_TITILANDO;
			}
			break;
		case EST_LED_ROJO_TITILANDO:
			if(timLed == 0)
			{
				board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_TOGGLE);
				timLed = 500;
				estMefControlLed = EST_LED_ROJO_TITILANDO;
			}
			if(led_titila == 0)
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


void runMode(void)
{
	if(ControlRun){
	CLOCK_SYS_SetConfiguration(&clockConfigRun);
	ControlRun = 0;
	ControlVlpr = 1;
	}
}

void vlprMode(void)
{
	if(ControlVlpr)
	{
	CLOCK_SYS_SetConfiguration(&clockConfigVlpr);
	ControlVlpr = 0;
	ControlRun = 1;
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
    if(timAcc)
    {
    	timAcc--;
    }
    if(timOff)
    {
    	timOff--;
    }

}

/*==================[end of file]============================================*/
