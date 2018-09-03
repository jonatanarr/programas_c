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
#include "key.h"

/*==================[macros and definitions]=================================*/

typedef enum
{
    ESPERANDO_ACTIVACION = 0,
    ESPERANDO_DESACTIVACION,
}estPul_enum;

/*==================[internal data declaration]==============================*/

static estPul_enum estSW;
static bool eventSW;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void key_init(void)
{
    estSW = ESPERANDO_ACTIVACION;
    eventSW = 0;
}

bool key_getPressEv(void)
{
    bool ret = 0;

    if (eventSW)
    {
        eventSW = 0;
        ret = 1;
    }

    return ret;
}

void key_periodicTask1ms(void)
{
    switch (estSW)
    {
        case ESPERANDO_ACTIVACION:
            if (board_getSw(BOARD_SW_ID_1))
            {
                eventSW = 1;
                estSW = ESPERANDO_DESACTIVACION;
            }
            break;

        case ESPERANDO_DESACTIVACION:
            if (!board_getSw(BOARD_SW_ID_1))
            {
                estSW = ESPERANDO_ACTIVACION;
            }
            break;

        default:
            estSW = ESPERANDO_ACTIVACION;
            break;
    }
}


/*==================[end of file]============================================*/
