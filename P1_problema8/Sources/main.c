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
#include "fsl_port_hal.h"

#include "fsl_adc16_hal.h"


// Project Included Files
#include "board.h"

/*==================[macros and definitions]=================================*/

#define ADC_SC1A_POINTER        0U
#define ADC_SC1B_POINTER        1U
#define ADC_CHANNEL_3           3U

#define SENSOR_LUZ_PORT         PORTE
#define SENSOR_LUZ_PIN          22


typedef enum
{
	ADC_INIT = 0,
	CONVIRTIENDO,
}estMefADC_enum;

typedef enum
{
	NIVEL_LUMINOSIDAD_INIT = 0,
	NIVEL_MUY_BAJA,
	NIVEL_MEDIA,
	NIVEL_ALTA,
}estMefLuminosidadLvl_enum;

typedef enum
{
	LED_INIT = 0,
	LED_RV_OFF,
	LED_ROJO_ON,
	LED_VERDE_ON,
	LED_RV_ON,
}estmefLed_enum;

/*==================[internal data declaration]==============================*/
static uint16_t result;
static int32_t EventLvl; // preguntar recome
static int32_t tim = 0;

/*==================[internal functions declaration]=========================*/
void ADC_Init(void);

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
void ADC_Init(void)
{
	adc16_converter_config_t adcUserConfig;

	CLOCK_SYS_EnableAdcClock(ADC0_IDX);

	// Select alternative clock for ADC0 from outdiv5
	// Lowest ADC Frequency.
	adcUserConfig.clkSrc = kAdc16ClkSrcOfBusClk;
	adcUserConfig.clkDividerMode = kAdc16ClkDividerOf8;
	// Reference voltage as Vadd.
	adcUserConfig.refVoltSrc = kAdc16RefVoltSrcOfVref;
	// Software trigger.
	adcUserConfig.hwTriggerEnable = false;

	// Select 12-bit single-end mode
	adcUserConfig.resolution = kAdc16ResolutionBitOf12or13;
	adcUserConfig.highSpeedEnable = true;
	adcUserConfig.continuousConvEnable = false;

	ADC16_HAL_ConfigConverter(ADC0, &adcUserConfig);

	PORT_HAL_SetMuxMode(SENSOR_LUZ_PORT, SENSOR_LUZ_PIN, kPortPinDisabled);
}

void ADC_IniciarConv(void)
{
	adc16_chn_config_t AdcChCfg;
	AdcChCfg.chnIdx = (adc16_chn_t)ADC_CHANNEL_3;
	AdcChCfg.convCompletedIntEnable = false;
	AdcChCfg.diffConvEnable = false;
	ADC16_HAL_ConfigChn(ADC0, ADC_SC1A_POINTER, &AdcChCfg);
}


int ADC_GetIntenLvl(uint16_t valor)
{
	int ret=0;

	if (valor >= 0 		&& 	valor <= 1365)	{ret = 2;}
	if (valor >= 1366 	&& 	valor <= 2730)	{ret = 1;}
	if (valor >= 2731	&& 	valor <= 4096)	{ret = 0;}
	return ret;
}

void mefADC(void)
{
	static estMefADC_enum estMefADC = ADC_INIT;
	switch (estMefADC)
	{
		case ADC_INIT:
			estMefADC= CONVIRTIENDO;
			break;
		case CONVIRTIENDO:
			if(ADC16_HAL_GetChnConvCompletedFlag(ADC0, ADC_SC1A_POINTER))
			{
				result = ADC16_HAL_GetChnConvValue(ADC0, ADC_SC1A_POINTER );
				ADC_IniciarConv();
			}

	}
}

void mefLuminosidadLvl(void)
{
	static estMefLuminosidadLvl_enum estMefLuminosidadLvl = NIVEL_LUMINOSIDAD_INIT;
	switch (estMefLuminosidadLvl)
	{
	case NIVEL_LUMINOSIDAD_INIT:
		estMefLuminosidadLvl = NIVEL_MUY_BAJA;
	case NIVEL_MUY_BAJA:
		if (ADC_GetIntenLvl(result)==1)
		{
			EventLvl = 1;
			estMefLuminosidadLvl = NIVEL_MEDIA;
		}
		break;
	case NIVEL_MEDIA:
		if (ADC_GetIntenLvl(result)==2)
		{
			EventLvl = 2;
			estMefLuminosidadLvl = NIVEL_ALTA;
		}
		break;
	case NIVEL_ALTA:
		if (ADC_GetIntenLvl(result)==0)
		{
			EventLvl = 0;
			estMefLuminosidadLvl = NIVEL_MUY_BAJA;
		}
		break;
	default:
		estMefLuminosidadLvl = NIVEL_MUY_BAJA;
		break;
	}
}

void mefLed(void)
{
	static estmefLed_enum estmefLed = LED_INIT;

	switch (estmefLed)
	{
		case LED_INIT:
			tim=2000;
			board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_OFF);
			board_setLed(BOARD_LED_ID_VERDE,BOARD_LED_MSG_OFF);
			estmefLed = LED_RV_OFF;

			break;
		case LED_RV_OFF:
			if (tim == 0)
			{
				board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_ON);
				board_setLed(BOARD_LED_ID_VERDE,BOARD_LED_MSG_OFF);
				estmefLed = LED_ROJO_ON;
			}
			break;
		case LED_ROJO_ON:
			if (EventLvl == 1)
			{
				board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_OFF);
				board_setLed(BOARD_LED_ID_VERDE,BOARD_LED_MSG_ON);
				estmefLed = LED_VERDE_ON;

			}
			break;
		case LED_VERDE_ON:
			if (EventLvl == 2)
			{
				board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_ON);
				board_setLed(BOARD_LED_ID_VERDE,BOARD_LED_MSG_ON);
				estmefLed = LED_RV_ON;
			}
			break;
		case LED_RV_ON:
			if (EventLvl == 0)
			{
				board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_ON);
				board_setLed(BOARD_LED_ID_VERDE,BOARD_LED_MSG_OFF);
				estmefLed = LED_ROJO_ON;
			}
			break;
		default:
			estmefLed = LED_ROJO_ON;
			break;
	}
}

/*==================[external functions definition]==========================*/

int main(void)
{
	// Se inicializan funciones de la placa
	board_init();
	// Se inicializa el timer PIT
	PIT_Init();
	// Se inicializa el ADC
	ADC_Init();
	// Se inicializa la conversion
	ADC_IniciarConv();

    while(1)
    {
    	mefADC();
    	mefLuminosidadLvl();
    	mefLed();
    }
}


void PIT_IRQHandler(void)
{
    PIT_HAL_ClearIntFlag(PIT, 1);

    if(tim)
    {
    	tim--;
    }
}

/*==================[end of file]============================================*/
