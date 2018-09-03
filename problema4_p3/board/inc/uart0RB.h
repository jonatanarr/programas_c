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

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

/*==================[inclusions]=============================================*/
#include "MKL46Z4.h"
#include "stdbool.h"
#include "stdint.h"

/*==================[cplusplus]==============================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions definition]==========================*/

/** \brief inicializa uart0 con buffer circular
**
** \param[in] rxBufSize tamaño del buffer de recepción
** \param[in] rxBufSize tamaño del buffer de transmisión
**
** \return true si se pudo inicializar la uart y los buffer
** false si no se pudo inicializar la uart y los buffer
**
** \comment esta función inicializa la en 115200 8N1
**/
bool uart0RB_init(uint16_t rxBufSize, uint16_t txBufSize);
/** \brief configura uart0
**
** \param[in] baudrate velocidad de comunicación
** \param[in] dataBits cantidad de bits de datos: 7 u 8
** \param[in] parBits bit de paridad: 0 sin
** 1 impar
** 2 par
** \param[in] stopBits cantidad de bits de stop 1 o 2
**
** \return true si se pudo configurar la uart
** false si no se pudo configurar la uart
**/
bool uart0RB_config(uint16_t baudrate, uint8_t dataBits,
uint8_t parBits, uint8_t stopBits);
/** \brief envía datos por uart0
**
** \param[in] pData puntero a los datos que hay que enviar
** \param[in] size cantidad de bytes a enviar
**
** \return cantidad de bytes enviados
**/
uint16_t uart0RB_send(int8_t *pData, uint16_t size);
/** \brief recive datos por uart0
**
** \param[in] pData puntero a donde dejar guardado los datos
** \param[in] size cantidad de bytes que se desean recibir
**
** \return cantidad de bytes recibidos
**/
uint16_t uart0RB_recv(int8_t *pData, uint16_t size);

/*==================[cplusplus]==============================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* RING_BUFFER_H_ */
