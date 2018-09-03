/* Copyright 2017, DSI FCEIA UNR - Sistemas Digitales 2
 *    DSI: http://www.dsi.fceia.unr.edu.ar/
 * Copyright 2017, Diego Alegrechi
 * Copyright 2017, 2018, Gustavo Muro
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

// SDK Included Files
#include "fsl_sim_hal.h"
#include "fsl_mcg_hal.h"
#include "fsl_tpm_hal.h"
#include "fsl_pit_hal.h"
#include "fsl_clock_manager.h"

// Project Included Files
#include "board.h"
#include "key.h"

/*==================[macros and definitions]=================================*/

#define CHANNEL_PWM_2				2
#define CHANNEL_PWM_5				5


/*==================[internal data declaration]==============================*/
int32_t cuenta = 0;
int32_t ValDuty = 0;

int32_t Duty100porciento;
int32_t DutyStep;

/*==================[internal functions declaration]=========================*/

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

void TPM_Init(void)
{
	uint32_t frecPerif;
	uint32_t channel;

	tpm_pwm_param_t ParamPwm; //aca se setea el modo de trabajo center o edge aligned

	ParamPwm.mode = kTpmEdgeAlignedPWM; // modo de pwm
	ParamPwm.edgeMode = kTpmLowTrue; // estado activo del ciclo

	// Habilito clock del periferico
	SIM_HAL_EnableClock(SIM, kSimClockGateTpm0);

	// Detengo cuenta del Timer
	TPM_HAL_SetClockMode(TPM0, kTpmClockSourceNoneClk);

	// Reseteo el timer
	TPM_HAL_Reset(TPM0, 0);

	// Clock prescaler = 1
	TPM_HAL_SetClockDiv(TPM0, kTpmDividedBy1);

	// Borro bandera de overflow
	TPM_HAL_ClearTimerOverflowFlag(TPM0);

	// Deshabilito todos los canales
	for (channel = 0; channel < FSL_FEATURE_TPM_CHANNEL_COUNT ; channel++)
	{
		TPM_HAL_DisableChn(TPM0, channel);
	}

	// NO Activo interupcion ante bandera de OverFlow
	TPM_HAL_DisableTimerOverflowInt(TPM0);

	// Se habilita IRC y se elige Frec alta
    CLOCK_HAL_SetInternalRefClkEnableCmd(MCG, true);
    CLOCK_HAL_SetInternalRefClkMode(MCG, kMcgIrcFast);

	// Elijo la fuente de clock para el TPM
	CLOCK_HAL_SetTpmSrc(SIM, 0, kClockTpmSrcMcgIrClk);

	// Obtengo Frecuencia IR MCG con la que alimento el TPM
	CLOCK_SYS_GetFreq(kMcgIrClock,&frecPerif);

	// Frec. de PWM 1KHz -- Periodo 1mseg
	Duty100porciento = frecPerif / 1000;
	DutyStep = Duty100porciento / 100;

	// Valor de cuenta max. -- Frec de PWM
	TPM_HAL_SetMod(TPM0, Duty100porciento);

	// Seteo el duty cycle inicial
	//TPM_HAL_SetChnCountVal(TPM0, CHANNEL_PWM_2, ValDuty);
	TPM_HAL_SetChnCountVal(TPM0, CHANNEL_PWM_5, ValDuty);


	// Habilito el canal PWM
	//TPM_HAL_EnablePwmMode(TPM0, &ParamPwm, CHANNEL_PWM_2);
	TPM_HAL_EnablePwmMode(TPM0, &ParamPwm, CHANNEL_PWM_5);


	// Habilito el Timer -- Fuente de clock interna
	TPM_HAL_SetClockMode(TPM0, kTpmClockSourceModuleClk);

	//board_ledOutPWM(BOARD_LED_ID_ROJO);
	board_ledOutPWM(BOARD_LED_ID_VERDE);

}

/*==================[external functions definition]==========================*/

int main(void)
{
	// Se inicializan funciones de la placa
	board_init();

	TPM_Init();
	PIT_Init();

    while(1)
    {
    	if (key_getPressEv(BOARD_SW_ID_1))
		{
    	    ValDuty = ValDuty + DutyStep;

    		if(ValDuty > Duty100porciento)
    			ValDuty = Duty100porciento;

			//TPM_HAL_SetChnCountVal(TPM0, CHANNEL_PWM_2, ValDuty);
			TPM_HAL_SetChnCountVal(TPM0, CHANNEL_PWM_5, ValDuty);

		}
    	if (key_getPressEv(BOARD_SW_ID_3))
		{
    	    ValDuty = ValDuty - DutyStep;

    		if(ValDuty < 0)
    		    ValDuty = 0;

    		//TPM_HAL_SetChnCountVal(TPM0, CHANNEL_PWM_2, ValDuty);
    		TPM_HAL_SetChnCountVal(TPM0, CHANNEL_PWM_5, ValDuty);

		}
    }
}

void PIT_IRQHandler(void)
{
	PIT_HAL_ClearIntFlag(PIT, 1);

	key_periodicTask1ms();
}

/*==================[end of file]============================================*/
