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
#include "board.h"
#include "fsl_clock_manager.h"

#include "fsl_sim_hal.h"
#include "fsl_mcg_hal.h"
#include "fsl_tpm_hal.h"
#include "fsl_smc_hal.h"
#include "MKL46Z4.h"
#include "fsl_port_hal.h"

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

/*==================[internal data declaration]==============================*/

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

/*==================[internal functions declaration]=========================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

void TPM_Init(void)
{
	uint32_t channel = 0;

	// Habilito clock del periferico
	SIM_HAL_EnableClock(SIM, kSimClockGateTpm0);

	// Detengo cuenta del Timer
	TPM_HAL_SetClockMode(TPM0, kTpmClockSourceNoneClk);

	// Reseteo el timer
	TPM_HAL_Reset(TPM0, 0);

	// Clock prescaler = 128
	TPM_HAL_SetClockDiv(TPM0, kTpmDividedBy128);

	// Borro bandera de overflow
	TPM_HAL_ClearTimerOverflowFlag(TPM0);

	// Configuro valor MOD a TPM_MOD_MOD_MASK (0xFFFF)
	TPM_HAL_SetMod(TPM0, TPM_MOD_MOD_MASK);

	// Uso TPM solo como timer -- Deshabilito todos los canales
	for (channel = 0; channel < FSL_FEATURE_TPM_CHANNEL_COUNT ; channel++)
	{
		TPM_HAL_DisableChn(TPM0, channel);
	}

	// Cuenta ascendente
	TPM_HAL_SetCpwms(TPM0, 0);

	// Activo interupcion ante bandera de OverFlow
	TPM_HAL_EnableTimerOverflowInt(TPM0);

	// Elijo la fuente de clock para el TPM (OSCERCLK)
	CLOCK_HAL_SetTpmSrc(SIM, 0, kClockTpmSrcOsc0erClk);

	// Habilito el Timer -- Fuente de clock interna
	TPM_HAL_SetClockMode(TPM0, kTpmClockSourceModuleClk);
}

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

    CLOCK_SYS_SetConfiguration(&clockConfigRun);
}

/*==================[external functions definition]==========================*/

int main(void)
{
	uint32_t i;

	board_init();

	// Se inicializa el clock del core
	ClockInit();

	// Se inicializa el timer
	TPM_Init();

	// Se habilitan interrupciones de TPM
	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_EnableIRQ(TPM0_IRQn);

	while (1)
	{
		// Delay por software
		i = 300000;
		while (i--);

		board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_TOGGLE);

		if (board_getSw(BOARD_SW_ID_1))
		{
			CLOCK_SYS_SetConfiguration(&clockConfigVlpr);
		}

		if (board_getSw(BOARD_SW_ID_3))
		{
			CLOCK_SYS_SetConfiguration(&clockConfigRun);
		}
    }
}

///////////////////////////////////////////////////////////////////////////////
// TPM0 IRQ
///////////////////////////////////////////////////////////////////////////////

void TPM0_IRQHandler(void)
{
	// Borro bandera de overflow
	TPM_HAL_ClearTimerOverflowFlag(TPM0);

	board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_TOGGLE);
}

/*==================[end of file]============================================*/
