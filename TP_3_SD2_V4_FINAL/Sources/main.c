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
#include "fsl_sim_hal.h"
#include "fsl_lpsci_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_pit_hal.h"
#include "fsl_port_hal.h"
#include "math.h"


#include "mma8451.h"
#include "ringBuffer.h"
#include "board_rs485.h"

/*==================[macros and definitions]=================================*/

typedef enum
{
    EST_ESPERANDO_DOS_PUNTOS = 0,
    EST_GUARDANDO_DATOS,
    EST_VALIDANDO_DATOS,
	EST_ACTUANDO_SOBRE_PERIF,
	EST_ARMANDO_RESPUESTA,
}estadoRecDatos_enum;

typedef enum
{
	EST_MEF_ENV_DATOS_INIT = 0,
	EST_MEF_ESPERANDO_ACTIVACION_RTA,
	EST_MEF_GENERANDO_TRAMA,
	EST_MEF_ENVIANDO_RESPUESTA,
}estMefEnvDatos_enum;

typedef enum
{
	EST_MEF_LED_INIT = 0,
	EST_MEF_LED_ESP_ACTIVANCION,
	EST_MEF_LED_SELECT,
}estMefAccionLed_enum;

typedef enum
{
	EST_MEF_ACC_INIT = 0,
	EST_MEF_ACC_ESP_ACTIVANCION,
	EST_MEF_ACC_TOMANDO_DATOS,
}estMefAccionAcc_enum;

typedef enum
{
	EST_MEF_SW_INIT = 0,
	EST_MEF_SW_ESP_ACTIVANCION,
	EST_MEF_SW_SELECT,
}estMefAccionSw_enum;

typedef enum
{
	EST_MEF_ESP_VERIFICACION = 0,
	EST_MEF_SELEC_PERIFERICOS,
}estMefAccionPerifericos_enum;


/*==================[internal data declaration]==============================*/
static int32_t timeOut;



static bool actMefSelectorPerif;
static bool actMefLed;
static bool actMefSw;
static bool actMefAcc;
static bool PerifericoListo;
static bool actMefRespuesta;
static bool RespuestaEnviada;

static uint8_t 	EventSw;


static uint8_t buffer[20];
static uint8_t indexRx;

static uint32_t Acc;

/*==================[internal functions declaration]=========================*/

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
    NVIC_SetPriority(PIT_IRQn,3);
}



/*==================[BORRADO DE BANDERAS]==========================*/

bool SelPerif_getEv(void)
{
    bool ret = false;

    if (actMefSelectorPerif)
    {
    	actMefSelectorPerif = 0;
        ret = true;
    }
    return ret;
}

bool Led_getEv(void)
{
    bool ret = false;

    if (actMefLed)
    {
    	actMefLed = 0;
        ret = true;
    }
    return ret;
}

bool Sw_getEv(void)
{
    bool ret = false;

    if (actMefSw)
    {
    	actMefSw = 0;
        ret = true;
    }
    return ret;
}

bool Acc_getEv(void)
{
    bool ret = false;

    if (actMefAcc)
    {
    	actMefAcc = 0;
        ret = true;
    }
    return ret;
}

bool PerifListo_getEv(void)
{
    bool ret = false;

    if (PerifericoListo)
    {
    	PerifericoListo = 0;
        ret = true;
    }
    return ret;
}

bool Respusta_getEv(void)
{
    bool ret = false;

    if (actMefRespuesta)
    {
    	actMefRespuesta	 = 0;
        ret = true;
    }
    return ret;
}

bool RespustaEnviada_getEv(void)
{
    bool ret = false;

    if (RespuestaEnviada)
    {
    	RespuestaEnviada	 = 0;
        ret = true;
    }
    return ret;
}




/*==================[FIN DE BORRADO DE BANDERAS]==========================*/


static bool validarDatos(uint8_t *pBuf, uint8_t size)
{
    bool ret = 1;

    if ((size != 4) && (size != 5))				{ret = 0;}
    if (pBuf[0] != '1' || pBuf[1] != '3')		{ret = 0;}


    if (size == 4)
    {
		if (pBuf[2] != '1' && pBuf[2] != '2')						{ret = 0;}
		if (pBuf[3] != '0' && pBuf[3] != '1' && pBuf[3] != '3')	 	{ret = 0;}
    }


    if (size == 5)
    {
		if (pBuf[2] != '0')						{ret = 0;}
		if (pBuf[3] != '1' && pBuf[3] != '2') 	{ret = 0;}
		if (pBuf[4] != 'E' && pBuf[4] != 'e' && pBuf[4] != 'A' && pBuf[4] != 'a' && pBuf[4] != 'T' && pBuf[4] != 't')	{ret = 0;}
    }
    return ret;
}

static void mefRecDatos(void)
{
    static estadoRecDatos_enum estadoRecDatos = EST_ESPERANDO_DOS_PUNTOS;
    uint8_t ret;
    uint8_t byteRec;

    switch (estadoRecDatos)
    {
        case EST_ESPERANDO_DOS_PUNTOS:
            ret = recDatos(&byteRec, 1);

            if (ret != 0 && byteRec == ':')
            {
                indexRx = 0;
                timeOut = 1000;
                estadoRecDatos = EST_GUARDANDO_DATOS;
            }
            break;

        case EST_GUARDANDO_DATOS:
            ret = recDatos(&byteRec, 1);

            if (ret != 0 && byteRec != ':' && byteRec != 0x0A)
            {
                buffer[indexRx] = byteRec;
                indexRx++;
                timeOut = 1000;
            }
            if (ret != 0 && byteRec == ':')
            {
                indexRx = 0;
                timeOut = 1000;
            }
            if (ret != 0 && byteRec == 0x0A)
            {
                estadoRecDatos = EST_VALIDANDO_DATOS;
            }
            if (timeOut == 0)
            {
                estadoRecDatos = EST_ESPERANDO_DOS_PUNTOS;
            }
            if (indexRx >= sizeof(buffer))
            {
                estadoRecDatos = EST_ESPERANDO_DOS_PUNTOS;
            }
            break;

        case EST_VALIDANDO_DATOS:
            if (validarDatos(&buffer[0], indexRx))
            {
            	actMefSelectorPerif = 1;
                estadoRecDatos = EST_ACTUANDO_SOBRE_PERIF;
            }
            if (!validarDatos(&buffer[0], indexRx))
            {
                estadoRecDatos = EST_ESPERANDO_DOS_PUNTOS;
            }
            break;

        case EST_ACTUANDO_SOBRE_PERIF:
        	if(PerifListo_getEv())
        	{
                actMefRespuesta		= 1;
                estadoRecDatos = EST_ARMANDO_RESPUESTA;
        	}
            break;
        case EST_ARMANDO_RESPUESTA:
        	if(RespustaEnviada_getEv())
        	{
                estadoRecDatos = EST_ESPERANDO_DOS_PUNTOS;
        	}
            break;
        default:
            estadoRecDatos = EST_ESPERANDO_DOS_PUNTOS;
            break;
    }
}

static void mefEnvDatos(void)
{
	static estMefEnvDatos_enum estMefEnvDatos = EST_MEF_ENV_DATOS_INIT;

	static uint8_t indexTx;
	static uint8_t indexEnv;

	static uint8_t Unidades;
	static uint8_t Decenas;
	static uint8_t Centenas;


	uint8_t env;
	uint8_t byteEnv;
	uint8_t i;

	static uint8_t bufferTx[20];


	switch (estMefEnvDatos)
	{
		case EST_MEF_ENV_DATOS_INIT:

			estMefEnvDatos = EST_MEF_ESPERANDO_ACTIVACION_RTA;
			break;
		case EST_MEF_ESPERANDO_ACTIVACION_RTA:
			if(Respusta_getEv())
			{
				estMefEnvDatos = EST_MEF_GENERANDO_TRAMA;

			}
			break;
		case EST_MEF_GENERANDO_TRAMA:
			bufferTx[0] = ':';
			/*Respuesta Leds*/
			if(indexRx == 5 && buffer[2] == '0')
			{
				for(i = 1; i < indexRx + 1 ; i++)
				{
					bufferTx[i] = buffer[i-1];
				}
				indexTx = indexRx + 2;
				bufferTx[indexTx - 1] = 0x0A;// ARREGLAR EL INDICE
				indexEnv = 0;
				estMefEnvDatos = EST_MEF_ENVIANDO_RESPUESTA;
			}

			/*Respuesta Sws*/
			if(indexRx == 4 && buffer[2] == '1')
			{
				for(i = 1; i < indexRx + 1 ; i++)
				{
					bufferTx[i] = buffer[i-1];
				}
				indexTx = indexRx + 3;
				bufferTx[indexTx - 2] = EventSw;
				bufferTx[indexTx - 1] = 0x0A;// ARREGLAR EL INDICE
				indexEnv = 0;
				estMefEnvDatos = EST_MEF_ENVIANDO_RESPUESTA;
			}
			/*Respuesta Acc*/
			if(indexRx == 4 && buffer[2] == '2')
			{
				for(i = 1; i < indexRx + 1 ; i++)
				{
					bufferTx[i] = buffer[i-1];
				}
				indexTx = indexRx + 5;

				Unidades 	= Acc % 10;//unidades
				Acc 		= Acc / 10;
				Decenas 	= Acc % 10;//decenas
				Acc 		= Acc / 10;
				Centenas 	= Acc % 10;// centenas

				bufferTx[indexTx - 4] = Centenas + '0';
				bufferTx[indexTx - 3] = Decenas  + '0';
				bufferTx[indexTx - 2] = Unidades + '0';
				bufferTx[indexTx - 1] = 0x0A;// ARREGLAR EL INDICE
				indexEnv = 0;
				estMefEnvDatos = EST_MEF_ENVIANDO_RESPUESTA;
			}
			break;
		case EST_MEF_ENVIANDO_RESPUESTA:
			if(indexEnv != indexTx)
			{
				byteEnv = bufferTx[indexEnv];
				env = envDatos(&byteEnv,1);
				indexEnv++;
			}
			if(indexEnv == indexTx && byteEnv == 0x0A)
			{
				RespuestaEnviada = 1;
				estMefEnvDatos = EST_MEF_ESPERANDO_ACTIVACION_RTA;
			}
			break;
		default:
			estMefEnvDatos = EST_MEF_ESPERANDO_ACTIVACION_RTA;
			break;
	}
}

static void mefAccionLed (void)
{
	static estMefAccionLed_enum estMefAccionLed = EST_MEF_LED_INIT;
	static uint8_t LedId;
	switch (estMefAccionLed)
	{
		case EST_MEF_LED_INIT:
			board_setLed(BOARD_LED_ID_ROJO,BOARD_LED_MSG_OFF);
			board_setLed(BOARD_LED_ID_VERDE,BOARD_LED_MSG_OFF);
			estMefAccionLed = EST_MEF_LED_ESP_ACTIVANCION;
			break;
		case EST_MEF_LED_ESP_ACTIVANCION:
			if(Led_getEv())
			{
				if(buffer[3] == '1')
				{
					LedId = BOARD_LED_ID_ROJO;
				}
				if(buffer[3] == '2')
				{
					LedId = BOARD_LED_ID_VERDE;
				}
				estMefAccionLed = EST_MEF_LED_SELECT;
			}
			break;
		case EST_MEF_LED_SELECT:
			if(buffer[4] == 'E')
			{
				board_setLed(LedId,BOARD_LED_MSG_ON);
			}
			if(buffer[4] == 'A')
			{
				board_setLed(LedId,BOARD_LED_MSG_OFF);
			}
			if(buffer[4] == 'T')
			{
				board_setLed(LedId,BOARD_LED_MSG_TOGGLE);
			}

			estMefAccionLed = EST_MEF_LED_ESP_ACTIVANCION;
			PerifericoListo = 1;
			break;
		default:
			estMefAccionLed = EST_MEF_LED_ESP_ACTIVANCION;
			break;
	}

}

static void mefAccionSw (void)
{
	static estMefAccionSw_enum estMefAccionSw = EST_MEF_SW_INIT;
	switch (estMefAccionSw)
	{
		case EST_MEF_SW_INIT:
			//EventSw = 'N';
			estMefAccionSw = EST_MEF_SW_ESP_ACTIVANCION;
			break;
		case EST_MEF_SW_ESP_ACTIVANCION:
			if(Sw_getEv())
			{
				estMefAccionSw = EST_MEF_LED_SELECT;
			}
			break;
		case EST_MEF_SW_SELECT:
			if(buffer[3] == '1')
			{
				if(board_getSw(BOARD_SW_ID_1))
					{
						EventSw = 'P';
					}
				else
					{
						EventSw = 'N';
					}
				estMefAccionSw = EST_MEF_SW_ESP_ACTIVANCION;
				PerifericoListo = 1;
			}
			if(buffer[3] == '3')
			{
				if(board_getSw(BOARD_SW_ID_3))
					{
						EventSw = 'P';
					}
				else
					{
						EventSw = 'N';
					}
				estMefAccionSw = EST_MEF_SW_ESP_ACTIVANCION;
				PerifericoListo = 1;
			}
			break;
		default:
			estMefAccionSw = EST_MEF_SW_ESP_ACTIVANCION;
			break;
	}

}

static void mefAccionAcc (void)
{
	static estMefAccionAcc_enum estMefAccionAcc = EST_MEF_ACC_INIT;
	static 	int16_t Acc_X;
	static 	int16_t Acc_Y;
	static	int16_t Acc_Z;

	switch (estMefAccionAcc)
	{
		case EST_MEF_ACC_INIT:
			estMefAccionAcc = EST_MEF_ACC_ESP_ACTIVANCION;
			break;
		case EST_MEF_ACC_ESP_ACTIVANCION:
			if(Acc_getEv())
			{
				estMefAccionAcc = EST_MEF_ACC_TOMANDO_DATOS;
			}
			break;
		case EST_MEF_ACC_TOMANDO_DATOS:
			if(buffer[3] == '0')
			{
				Acc_X = mma8451_getAcX();
				Acc_Y = mma8451_getAcY();
				Acc_Z = mma8451_getAcZ();
				Acc = Acc_X*Acc_X + Acc_Y*Acc_Y + Acc_Z*Acc_Z;
				Acc = sqrt(Acc);
				estMefAccionAcc = EST_MEF_ACC_ESP_ACTIVANCION;
				PerifericoListo = 1;
			}
			break;
		default:
			estMefAccionAcc = EST_MEF_ACC_ESP_ACTIVANCION;
			break;
	}
}

static void mefAccionPerifericos (void)
{
	static estMefAccionPerifericos_enum estMefAccionPerifericos = EST_MEF_ESP_VERIFICACION;
	switch (estMefAccionPerifericos)
	{
		case EST_MEF_ESP_VERIFICACION:
			if(SelPerif_getEv())
			{
				estMefAccionPerifericos = EST_MEF_SELEC_PERIFERICOS;
			}
			break;
		case EST_MEF_SELEC_PERIFERICOS:
			if(indexRx == 5 && buffer[2] == '0')
			{
				actMefLed = 1;
				estMefAccionPerifericos = EST_MEF_ESP_VERIFICACION;

			}
			if(indexRx == 4 && buffer[2] == '1')
			{
				actMefSw = 1;
				estMefAccionPerifericos = EST_MEF_ESP_VERIFICACION;

			}
			if(indexRx == 4 && buffer[2] == '2')
			{
				actMefAcc = 1;
				estMefAccionPerifericos = EST_MEF_ESP_VERIFICACION;
			}

			break;
		default:
			estMefAccionPerifericos = EST_MEF_ESP_VERIFICACION;
			break;
	}

}


/*==================[external functions definition]==========================*/


int main(void)
{
    board_init();

    PIT_Init();

    board_rs485_init();

    mma8451_init();

    while(1)
    {
        mefRecDatos();
        mefAccionPerifericos();
        mefAccionLed();
        mefAccionSw();
        mefAccionAcc();
        mefEnvDatos();
    }
}

void PIT_IRQHandler(void)
{
    PIT_HAL_ClearIntFlag(PIT, 1);

    if (timeOut)
    {
       timeOut--;

    }
}

/*==================[end of file]============================================*/




