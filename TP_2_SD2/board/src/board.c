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

#include "fsl_clock_manager.h"
#include "fsl_clock_MKL46Z4.h"

#include "fsl_sim_hal.h"
#include "fsl_mcg_hal.h"
#include "fsl_sim_hal_MKL46Z4.h"
#include "fsl_smc_hal.h"

#include "fsl_port_hal.h"
#include "fsl_gpio_hal.h"

#include "fsl_i2c_hal.h"
#include "mma8451.h"

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




/*----------------BOARD----------------*/
static const board_gpioInfo_type board_gpioLeds[] =
{
    {PORTE, GPIOE, 29},     /* LED ROJO */
    {PORTD, GPIOD, 5},      /* LED VERDE */
};


static const board_gpioInfo_type board_gpioSw[] =
{
    {PORTC, GPIOC, 3},      /* SW1 */
    {PORTC, GPIOC, 12},     /* SW3 */
};





/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
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


/*==================[external functions definition]==========================*/
void board_init(void)
{
    int32_t i;

    /* Activación de clock para los puertos utilizados */
    SIM_HAL_EnableClock(SIM, kSimClockGatePortA);
    SIM_HAL_EnableClock(SIM, kSimClockGatePortC);
    SIM_HAL_EnableClock(SIM, kSimClockGatePortD);
    SIM_HAL_EnableClock(SIM, kSimClockGatePortE);

    /* inicialización de leds */
    for (i = 0 ; i < BOARD_LED_ID_TOTAL ; i++)
    {
        PORT_HAL_SetMuxMode(board_gpioLeds[i].port, board_gpioLeds[i].pin, kPortMuxAsGpio);
        board_setLed(i, BOARD_LED_MSG_OFF);
        GPIO_HAL_SetPinDir(board_gpioLeds[i].gpio, board_gpioLeds[i].pin, kGpioDigitalOutput);
    }

    /* inicialización de SWs */
    for (i = 0 ; i < BOARD_SW_ID_TOTAL ; i++)
    {
        PORT_HAL_SetMuxMode(board_gpioSw[i].port, board_gpioSw[i].pin, kPortMuxAsGpio);
        GPIO_HAL_SetPinDir(board_gpioSw[i].gpio, board_gpioSw[i].pin, kGpioDigitalInput);
        PORT_HAL_SetPullCmd(board_gpioSw[i].port, board_gpioSw[i].pin, true);
        PORT_HAL_SetPullMode(board_gpioSw[i].port, board_gpioSw[i].pin, kPortPullUp);
    }
    /* =================== CLOCK INIT =================== */

    ClockInit();


    /* =================== PIT =================== */

    PIT_Init();


    /* =================== I2C =================== */

	/* seleccion función alternativa 5 (I2C) */
	PORT_HAL_SetMuxMode(PORTE, 24,kPortMuxAlt5);

	/* seleccion función alternativa 5 (I2C) */
	PORT_HAL_SetMuxMode(PORTE, 25,kPortMuxAlt5);

	/* activa clock para I2C */
	SIM_HAL_EnableClock(SIM, kSimClockGateI2c0);

	/* inicializa el I2C */
	I2C_HAL_Init(I2C0);

	/* configura baudrate */
	I2C_HAL_SetBaudRate(I2C0, SystemCoreClock, 100, NULL);

	/* activa el I2C */
	I2C_HAL_Enable(I2C0);

	/* =================== MMA8451 =================== */

	mma8451_FreeFallMode_init();
}

void board_setLed(board_ledId_enum id, board_ledMsg_enum msg)
{
    switch (msg)
    {
        case BOARD_LED_MSG_OFF:
            GPIO_HAL_SetPinOutput(board_gpioLeds[id].gpio, board_gpioLeds[id].pin);
            break;

        case BOARD_LED_MSG_ON:
            GPIO_HAL_ClearPinOutput(board_gpioLeds[id].gpio, board_gpioLeds[id].pin);
            break;

        case BOARD_LED_MSG_TOGGLE:
            GPIO_HAL_TogglePinOutput(board_gpioLeds[id].gpio, board_gpioLeds[id].pin);
            break;

        default:
            break;
    }
}

bool board_getSw(board_swId_enum id)
{
    return !GPIO_HAL_ReadPinInput(board_gpioSw[id].gpio, board_gpioSw[id].pin);
}

/*==================[end of file]============================================*/
