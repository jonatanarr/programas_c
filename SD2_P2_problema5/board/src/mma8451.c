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
#include "mma8451.h"
#include "fsl_i2c_hal.h"
#include "fsl_port_hal.h"
#include "fsl_gpio_hal.h"

/*==================[macros and definitions]=================================*/
#define MMA8451_I2C_ADDRESS     (0x1d)

#define INT1_PORT       PORTC
#define INT1_GPIO       GPIOC
#define INT1_PIN        5


typedef union
{
    struct
    {
        unsigned ACTIVE:1;
        unsigned F_READ:1;
        unsigned LNOISE:1;
        unsigned DR:3;
        unsigned ASLP_RATE:2;
    };
    uint8_t data;
}CTRL_REG1_t;

#define CTRL_REG1_ADDRESS   0X2A

typedef union
{
    struct
    {
        unsigned INT_EN_DRDY:1;
        unsigned :1;
        unsigned INT_EN_FF_MT:1;
        unsigned INT_EN_PULSE:1;
        unsigned INT_EN_LNDPRT:1;
        unsigned INT_EN_TRANS:1;
        unsigned INT_EN_FIFO:1;
        unsigned INT_EN_ASLP:1;
    };
    uint8_t data;
}CTRL_REG4_t;

#define CTRL_REG4_ADDRESS   0X2D

typedef union
{
    struct
    {
        unsigned INT_CFG_DRDY:1;
        unsigned :1;
        unsigned INT_CFG_FF_MT:1;
        unsigned INT_CFG_PULSE:1;
        unsigned INT_CFG_LNDPRT:1;
        unsigned INT_CFG_TRANS:1;
        unsigned INT_CFG_FIFO:1;
        unsigned INT_CFG_ASLP:1;
    };
    uint8_t data;
}CTRL_REG5_t;

#define CTRL_REG5_ADDRESS   0X2E

#define INT_SOURCE_ADDRESS   0X0C
#define STATUS_ADDRESS       0X00

/*==================[internal data declaration]==============================*/

static int16_t readX, readY, readZ;

/*==================[internal functions declaration]=========================*/
static uint8_t mma8451_read_reg(uint8_t addr)
{
    uint8_t ret;
    	
    I2C_HAL_MasterReceiveDataPolling(I2C0,
                    MMA8451_I2C_ADDRESS,
                    &addr,
                    1,
                    &ret,
                    1);
	
	return ret;
}

static void mma8451_write_reg(uint8_t addr, uint8_t data)
{
	I2C_HAL_MasterSendDataPolling(I2C0,
	                MMA8451_I2C_ADDRESS,
	                &addr,
	                1,
	                &data,
	                1);
}

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void mma8451_init(void)
{
    CTRL_REG1_t ctrl_reg1;
    CTRL_REG4_t ctrl_reg4;
    CTRL_REG5_t ctrl_reg5;

	ctrl_reg4.INT_EN_DRDY = 1;
	ctrl_reg4.INT_EN_FF_MT = 0;
	ctrl_reg4.INT_EN_PULSE = 0;
	ctrl_reg4.INT_EN_LNDPRT = 0;
	ctrl_reg4.INT_EN_TRANS = 0;
	ctrl_reg4.INT_EN_FIFO = 0;
	ctrl_reg4.INT_EN_ASLP = 0;

	mma8451_write_reg(CTRL_REG4_ADDRESS, ctrl_reg4.data);

	/* verificación */
	ctrl_reg4.data = mma8451_read_reg(CTRL_REG4_ADDRESS);

	ctrl_reg5.INT_CFG_DRDY = 1;
	ctrl_reg5.INT_CFG_FF_MT = 0;
	ctrl_reg5.INT_CFG_PULSE = 0;
	ctrl_reg5.INT_CFG_LNDPRT = 0;
	ctrl_reg5.INT_CFG_TRANS = 0;
	ctrl_reg5.INT_CFG_FIFO = 0;
	ctrl_reg5.INT_CFG_ASLP = 0;

	mma8451_write_reg(CTRL_REG5_ADDRESS, ctrl_reg5.data);

	/* verificación */
	ctrl_reg5.data = mma8451_read_reg(CTRL_REG5_ADDRESS);

	ctrl_reg1.ACTIVE = 1;
	ctrl_reg1.F_READ = 0;
	ctrl_reg1.LNOISE = 1;
	ctrl_reg1.DR = 0B101;
	ctrl_reg1.ASLP_RATE = 0B00;

    mma8451_write_reg(CTRL_REG1_ADDRESS, ctrl_reg1.data);

    /* verificación */
    ctrl_reg1.data = mma8451_read_reg(CTRL_REG1_ADDRESS);

    /* configuración de pin de interrupción */
    PORT_HAL_SetMuxMode(INT1_PORT, INT1_PIN, kPortMuxAsGpio);
    GPIO_HAL_SetPinDir(INT1_GPIO, INT1_PIN, kGpioDigitalInput);

    /* Interrupt polarity active high, or active low. Default value: 0.
       0: Active low; 1: Active high. VER REGISTRO CTRL_REG3 */
    PORT_HAL_SetPinIntMode(INT1_PORT, INT1_PIN, kPortIntLogicZero);

    NVIC_EnableIRQ(PORTC_PORTD_IRQn);
    NVIC_SetPriority(PORTC_PORTD_IRQn, 0);
}

void mma8451_setDataRate(DR_enum rate)
{
    CTRL_REG1_t ctr_reg1;
    bool estAct;

    /* antes de modificar data rate es necesario poner ACTIVE = 0 */
    ctr_reg1.data = mma8451_read_reg(CTRL_REG1_ADDRESS);

    /* guarda valor que tiene ACTIVE y luego pone a cero */
    estAct = ctr_reg1.ACTIVE;
    ctr_reg1.ACTIVE = 0;

	mma8451_write_reg(CTRL_REG1_ADDRESS, ctr_reg1.data);

	/* actualiza DR y en la misma escritura va a restaurar ACTIVE */
	ctr_reg1.DR = rate;
	ctr_reg1.ACTIVE = estAct;

	mma8451_write_reg(CTRL_REG1_ADDRESS, ctr_reg1.data);

	/* verificación */
	ctr_reg1.data = mma8451_read_reg(0x2a);
}

int16_t mma8451_getAcX(void)
{
	return (int16_t)(((int32_t)readX * 100) / (int32_t)4096);
}

int16_t mma8451_getAcY(void)
{
	return (int16_t)(((int32_t)readY * 100) / (int32_t)4096);
}

int16_t mma8451_getAcZ(void)
{
	return (int16_t)(((int32_t)readZ * 100) / (int32_t)4096);
}

void PORTC_PORTD_IRQHandler(void)
{
    int16_t readG;
    uint8_t temp8;

    temp8 = mma8451_read_reg(INT_SOURCE_ADDRESS);

    if (temp8 & 1)
    {
        temp8 = mma8451_read_reg(STATUS_ADDRESS);

        if (temp8 & 0b001)
        {
            readG   = (int16_t)mma8451_read_reg(0x01)<<8;
            readG  |= mma8451_read_reg(0x02);
            readX = readG >> 2;
        }

        if (temp8 & 0b010)
        {
            readG   = (int16_t)mma8451_read_reg(0x03)<<8;
            readG  |= mma8451_read_reg(0x04);
            readY = readG >> 2;
        }

        if (temp8 & 0b100)
        {
            readG   = (int16_t)mma8451_read_reg(0x05)<<8;
            readG  |= mma8451_read_reg(0x06);
            readZ = readG >> 2;
        }
    }

    PORT_HAL_ClearPinIntFlag(INT1_PORT, INT1_PIN);
}

/*==================[end of file]============================================*/
