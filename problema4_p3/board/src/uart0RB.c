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
#include "fsl_port_hal.h"
#include "fsl_gpio_hal.h"
#include "fsl_sim_hal.h"
#include "uart0RB.h"
#include "stdlib.h"
#include "fsl_clock_manager.h"
#include "fsl_lpsci_hal.h"
#include "fsl_uart_hal.h"




/*==================[macros and definitions]=================================*/
static uint32_t frecPerifUart;
static void* pRingBufferRx;
static void* pRingBufferTx;


typedef struct
{
    int32_t indexRead;
    int32_t indexWrite;
    int32_t count;
    int32_t size;
    uint8_t *pBuf;
}ringBuferData_struct;

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

void *ringBuffer_init(uint16_t size);
bool ringBuffer_putData(void *pRb, uint8_t data);
bool ringBuffer_getData(void *pRb, uint8_t *data);
bool ringBuffer_isFull(void *pRb);
bool ringBuffer_isEmpty(void *pRb);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
bool uart0RB_init(uint16_t rxBufSize, uint16_t txBufSize)
{
    uint32_t frecPerifUart;

    /*Inicializo la uart0*/
    PORT_HAL_SetMuxMode(PORTA, 1u, kPortMuxAlt2);
    PORT_HAL_SetMuxMode(PORTA, 2u, kPortMuxAlt2);

    // LPSCI: Low Power Serial Communication Interface

    /* selecciona clock de PLLFLLSEL */
    CLOCK_HAL_SetLpsciSrc(SIM, 0, kClockLpsciSrcPllFllSel);

    /* habilita clock a la UART0 (Lpsci0) */
    SIM_HAL_EnableClock(SIM, kSimClockGateLpsci0);

    CLOCK_SYS_GetFreq(kMcgOutClock, &frecPerifUart);

    /* setea baudrate */
    LPSCI_HAL_SetBaudRate(UART0, frecPerifUart, 115200);

    /* configura 8 bits de datos */
    LPSCI_HAL_SetBitCountPerChar(UART0, kLpsci8BitsPerChar);

    /* deshabilita paridad */
    LPSCI_HAL_SetParityMode(UART0, kLpsciParityDisabled);

    /* 1 bit de stop */
    LPSCI_HAL_SetStopBitCount(UART0, kLpsciOneStopBit);

    // Habilita interrupción de recepción
    LPSCI_HAL_SetIntMode(UART0, kLpsciIntRxDataRegFull, true);

    /* habilita transmisor y receptor */
    LPSCI_HAL_EnableTransmitter(UART0);
    LPSCI_HAL_EnableReceiver(UART0);

    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);

    /*ring buffer Rx*/
    pRingBufferRx = ringBuffer_init(rxBufSize);
    /*ring buffer Tx*/
    pRingBufferTx = ringBuffer_init(txBufSize);

    /*Hay que hacer el RETURN para verificar la configuracion*/
    return true;

}

bool uart0RB_config(uint16_t baudrate, uint8_t dataBits,uint8_t parBits, uint8_t stopBits)
{

    /* setea baudrate */
    LPSCI_HAL_SetBaudRate(UART0, frecPerifUart, baudrate);

    /* configura 8 bits de datos */
    LPSCI_HAL_SetBitCountPerChar(UART0, dataBits);

    /* deshabilita paridad */
    LPSCI_HAL_SetParityMode(UART0, parBits);

    /* 1 bit de stop */
    LPSCI_HAL_SetStopBitCount(UART0, stopBits);

    /*Hay que hacer el RETURN para verificar la configuracion*/
    return true;
}

uint16_t uart0RB_send(int8_t *pData, uint16_t size)
{
    int32_t ret = 0;

    /* entra sección de código crítico */
    NVIC_DisableIRQ(UART0_IRQn);

    /* si el buffer estaba vacío hay que habilitar la int TX */
    if (ringBuffer_isEmpty(pRingBufferTx))
        LPSCI_HAL_SetIntMode(UART0, kLpsciIntTxDataRegEmpty, true);

    while (!ringBuffer_isFull(pRingBufferTx) && ret < size)
    {
        ringBuffer_putData(pRingBufferTx, pData[ret]);
        ret++;
    }

    /* sale de sección de código crítico */
    NVIC_EnableIRQ(UART0_IRQn);

    return ret;
}

uint16_t uart0RB_recv(uint8_t *pData, uint16_t size)
{
    int32_t ret = 0;

    /* entra sección de código crítico */
    NVIC_DisableIRQ(UART0_IRQn);

    while (!ringBuffer_isEmpty(pRingBufferRx) && ret < size)//
    {
        ringBuffer_getData(pRingBufferRx, &pData[ret]);
        ret++;
    }

    /* sale de sección de código crítico */
    NVIC_EnableIRQ(UART0_IRQn);

    return ret;
}



	/*Configuraciones del RING BUFFER*/
void *ringBuffer_init(uint16_t size)
{
    ringBuferData_struct *rb;

    /* TODO: verificar puntero null*/
    rb = malloc(sizeof(ringBuferData_struct));

    /* TODO: verificar puntero null*/
    rb->pBuf = malloc(size);

    rb->indexRead = 0;
    rb->indexWrite = 0;
    rb->count = 0;
    rb->size = size;

    return rb;
}

bool ringBuffer_putData(void *pRb, uint8_t data)
{
    ringBuferData_struct *rb = pRb;
    bool ret = true;

    rb->pBuf[rb->indexWrite] = data;

    rb->indexWrite++;
    if (rb->indexWrite >= rb->size)
        rb->indexWrite = 0;

    if (rb->count < rb->size)
    {
        rb->count++;
    }
    else
    {
        /* si el buffer está lleno incrementa en uno indexRead
         * haciendo que se pierda el dato más viejo y devuelve
         * true para indicar que se estan perdiendo datos */
        rb->indexRead++;
        if (rb->indexRead >= rb->size)
            rb->indexRead = 0;
        	ret = false;
    }

    return ret;
}

bool ringBuffer_getData(void *pRb, uint8_t *data)
{
    ringBuferData_struct *rb = pRb;
    bool ret = true;

    if (rb->count)
    {
        *data = rb->pBuf[rb->indexRead];

        rb->indexRead++;
        if (rb->indexRead >= rb->size)
            rb->indexRead = 0;
        rb->count--;
    }
    else
    {
        ret = false;
    }

    return ret;
}

bool ringBuffer_isFull(void *pRb)
{
    ringBuferData_struct *rb = pRb;

    return rb->count == rb->size;
}

bool ringBuffer_isEmpty(void *pRb)
{
    ringBuferData_struct *rb = pRb;

    return rb->count == 0;
}

void UART0_IRQHandler(void)
{
    uint8_t data;
    // recepcion
    if (LPSCI_HAL_GetStatusFlag(UART0, kLpsciRxDataRegFull) &&
        LPSCI_HAL_GetIntMode(UART0, kLpsciIntRxDataRegFull))
    {
        /* obtiene dato recibido por puerto serie */
        LPSCI_HAL_Getchar(UART0, &data);

        /* pone dato en ring buffer */
        ringBuffer_putData(pRingBufferRx, data);

        LPSCI_HAL_ClearStatusFlag(UART0, kLpsciRxDataRegFull);	//blanqueo pedido por recepcion
    }
    // transmision
    if (LPSCI_HAL_GetStatusFlag(UART0, kLpsciTxDataRegEmpty) &&
        LPSCI_HAL_GetIntMode(UART0, kLpsciIntTxDataRegEmpty))
    {
        if (ringBuffer_getData(pRingBufferTx, &data))
        {
            /* envía dato extraído del RB al puerto serie */
            LPSCI_HAL_Putchar(UART0, data);
        }
        else
        {
            /* si el RB está vacío deshabilita interrupción TX */
            LPSCI_HAL_SetIntMode(UART0, kLpsciIntTxDataRegEmpty, false);	//deshabilita la interrupcion porque no hay datos para enviar queda deshabilitada para siempre
        }

        LPSCI_HAL_ClearStatusFlag(UART0, kLpsciTxDataRegEmpty);
    }
}

/*==================[end of file]============================================*/
