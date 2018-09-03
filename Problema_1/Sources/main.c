/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_device_registers.h"
#include "board.h"
#include "key.h"

//----------variables----------------
static int i = 0;
enum estados {prender, apagar};
enum estados estado_Sw = prender;


//----------funciones-----------------

int main(void)
{
	//Se inicializan funciones de la placa
	board_init();

	//Se inicializa el keyboard
	key_init();


    for (;;) {
    	i++;
    	key_periodicTask1ms();
    	mef_Led();
}
    return 0;
}

void mef_Led(void)
{
	static estMefLed_enum estMefLed= LED_INIT;

	switch (estMefLed)
	{
	case LED_INIT:
		board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_OFF);
		estMefLed = LED_ESPERANDO_EV;
		break;
	case LED_ESPERANDO_EV:
		if(key_getPressEv())
			board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_TOGGLE);
	}

}



