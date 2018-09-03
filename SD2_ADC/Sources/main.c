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

/*==================[internal data declaration]==============================*/
uint16_t result;

/*==================[internal functions declaration]=========================*/
void ADC_Init(void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

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

/*==================[external functions definition]==========================*/

int main(void)
{
	// Se inicializan funciones de la placa
	board_init();

	// Se inicializa el ADC
	ADC_Init();
	// Se inicializa la conversion
	ADC_IniciarConv();

    while(1)
    {
    	if(ADC16_HAL_GetChnConvCompletedFlag(ADC0, ADC_SC1A_POINTER))
    	{
    		result = ADC16_HAL_GetChnConvValue(ADC0, ADC_SC1A_POINTER );
    		ADC_IniciarConv();
    	}
    }
}

/*==================[end of file]============================================*/
