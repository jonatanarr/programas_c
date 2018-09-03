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
#include "fsl_clock_manager.h"
#include "fsl_pit_hal.h"
#include "fsl_lpsci_hal.h"


/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
static bool flagDatoRx;
static uint8_t rxChar;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

int main(void)
{

	// Enable clock for PORTs, setup board clock source, config pin
    board_init();


    // Enable the UART transmitter and receiver

    LPSCI_HAL_SetIntMode(UART0, kLpsciIntRxDataRegFull, true);

    /* habilita transmisor y receptor */
    LPSCI_HAL_EnableTransmitter(UART0);
    LPSCI_HAL_EnableReceiver(UART0);



	NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);


    while(true)
    {
    	if(flagDatoRx)
    	{
    		flagDatoRx = false;
    		LPSCI_HAL_Putchar(UART0, rxChar);
    	}
    }

}


void UART0_IRQHandler(void)
{
	LPSCI_HAL_ClearStatusFlag(UART0,kLpsciIntRxActiveEdge);
	LPSCI_HAL_Getchar(UART0, &rxChar);
	flagDatoRx = true;
}
/*==================[end of file]============================================*/
















