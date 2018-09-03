
/*==================[inclusions]=============================================*/

// Standard C Included Files
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
// SDK Included Files
#include "fsl_sim_hal.h"
#include "fsl_adc16_hal.h"
#include "fsl_lpsci_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_pit_hal.h"
#include "fsl_port_hal.h"
#include "MKL46Z4.h"
#include "fsl_gpio_hal.h"

// Project Included Files
#include "board.h"

/*==================[macros and definitions]=================================*/

#define ADC_SC1A_POINTER        0U
#define ADC_SC1B_POINTER        1U
#define ADC_CHANNEL_3           3U

/*==================[internal data declaration]==============================*/
uint16_t result;

/*==================[internal functions declaration]=========================*/
void ADC_Init(void);

/*==================[internal data definition]===============================*/

int16_t luzPorcentaje;
uint8_t caracterLeido;

uint8_t FLAG_DATO_RX = false;
uint16_t luzMaximo=160,luzMinimo=4095,apretoB=0,esperandoTecla=0,detuvoTransmision=0;
char Buffer[30];

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

void clrscr()	/* clear screen */
{
	sprintf(Buffer,"%c",12);
	LPSCI_HAL_SendDataPolling(UART0, (uint8_t *)Buffer,strlen(Buffer));
}

void PIT_Init(void)
{
	uint32_t FREC_PERIF;

	//PIT --> Clocked by bus clock frequency
	CLOCK_SYS_GetFreq(kBusClock,&FREC_PERIF);

	SIM_HAL_EnableClock(SIM, kSimClockGatePit0);
	PIT_HAL_Enable(PIT);
	PIT_HAL_SetTimerPeriodByCount(PIT, 1, FREC_PERIF/4);
	PIT_HAL_SetIntCmd(PIT, 1, true);
	PIT_HAL_SetTimerRunInDebugCmd(PIT, false);
	PIT_HAL_StartTimer(PIT, 1);
}

void ADC_Init(void)
{
	adc16_converter_config_t adcUserConfig;

	// Select alternative clock for ADC0 from outdiv5
	// Lowest ADC Frequency.
	adcUserConfig.clkSrc = kAdc16ClkSrcOfBusClk;	//Bus Clock
	adcUserConfig.clkDividerMode = kAdc16ClkDividerOf8;
	// Reference voltage as Vadd.
	adcUserConfig.refVoltSrc = kAdc16RefVoltSrcOfVref;
	// Software trigger.
	adcUserConfig.hwTriggerEnable = false;

	// Select 12-bit single-end mode
	adcUserConfig.resolution = kAdc16ResolutionBitOf12or13;
	adcUserConfig.highSpeedEnable = true;
	adcUserConfig.continuousConvEnable = false;

	ADC16_HAL_ConfigConverter(ADC0, &adcUserConfig);
}

void ADC_IniciarConv(void)
{
	adc16_chn_config_t AdcChCfg;
	AdcChCfg.chnIdx = (adc16_chn_t)ADC_CHANNEL_3;
	AdcChCfg.convCompletedIntEnable = true;
	AdcChCfg.diffConvEnable = false;
	ADC16_HAL_ConfigChn(ADC0, ADC_SC1A_POINTER, &AdcChCfg);
}

void lecturaLuz(void)
{
	//Máximo result=160 - Mínimo result=4095
	if(ADC16_HAL_GetChnConvCompletedFlag(ADC0, ADC_SC1A_POINTER))
	{
		result = ADC16_HAL_GetChnConvValue(ADC0, ADC_SC1A_POINTER );
		ADC_IniciarConv();
	}
	//calculo porcentaje de luz (100% result=160 - 0% result=4095)
	luzPorcentaje=(result-luzMinimo)*100/(luzMaximo-luzMinimo);
}

void menu(void)
{
	clrscr();
	char Menu[500];
	sprintf(Menu,"MENU:\r\nA.- Intensidad luminica actual\r\n"
			"B.- Intensidad luminica continua (Fin de Transmision al apretar un pulsador)\r\n"
			"C.- Cambiar de estado el Led Rojo\r\nD.- Retransmitir Menu\r\n");
	LPSCI_HAL_SendDataPolling(UART0, (uint8_t *)Menu,strlen(Menu));
}

void menuConIntensidad(void)
{
	clrscr();
	char Menu[500];
	sprintf(Menu,"MENU:\r\nA.- Intensidad luminica actual: %d%c\r\n"
			"B.- Intensidad luminica continua (Fin de Transmision al apretar un pulsador)\r\n"
			"C.- Cambiar de estado el Led Rojo\r\nD.- Retransmitir Menu\r\n",luzPorcentaje,'%');
	LPSCI_HAL_SendDataPolling(UART0, (uint8_t *)Menu,strlen(Menu));
}


void lecturaCaracter(void)
{

	switch (caracterLeido)
	{
		case 'A':
		case 'a':

			menuConIntensidad();

			break;

		case 'B':
		case 'b':

			apretoB=1;

			break;

		case 'C':
		case 'c':

			ledRojo_toggle();

			break;

		case 'D':
		case 'd':

			clrscr();
			menu();

			break;

		default:
			break;

	}

}


/*==================[external functions definition]==========================*/

int main(void)
{
	// Se inicializan funciones de la placa
	board_init();

	// Se inicializa el ADC
	ADC_Init();
	// Se inicializa la conversion
	ADC_IniciarConv();

    // Enable the UART transmitter and receiver

    LPSCI_HAL_SetIntMode(UART0, kLpsciIntRxDataRegFull, true);

    /* habilita transmisor y receptor */
    LPSCI_HAL_EnableTransmitter(UART0);
    LPSCI_HAL_EnableReceiver(UART0);

    PIT_Init();

    /* habilita interrupciones*/
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_EnableIRQ(PIT_IRQn);

    PORT_HAL_SetPinIntMode(PORTC, 3,kPortIntFallingEdge);
    PORT_HAL_SetPinIntMode(PORTC, 12,kPortIntFallingEdge);

    NVIC_ClearPendingIRQ(PORTC_PORTD_IRQn);
    NVIC_EnableIRQ(PORTC_PORTD_IRQn);

    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);

    NVIC_ClearPendingIRQ(ADC0_IRQn);
    NVIC_EnableIRQ(ADC0_IRQn);

    menu();
    while(1)
    {
    	/* si se detuvo la transmision (luego de envio continuo), espera ingreso y vuelve al menu */
    	if(detuvoTransmision&&FLAG_DATO_RX)
    	{
     			clrscr();
    			menu();
    			FLAG_DATO_RX=false;
    			detuvoTransmision=0;
    			apretoB=0;

    	}
    	/* si se esta en el menu, espera ingreso */
    	if(FLAG_DATO_RX&&!detuvoTransmision&&!apretoB)
    	{
    	    FLAG_DATO_RX = false;

    	    lecturaCaracter();
    	}

    }
}

void PORTC_PORTD_IRQHandler(void)
{
		PORT_HAL_ClearPortIntFlag(PORTC);
		/* si se apreto 'B' y se aprieta sw1 o sw3, detiene la transmision y envia mensaje pertinente */
		if(apretoB)
		{
			detuvoTransmision=1;
			apretoB=0;
			sprintf(Buffer,"Fin de Transmision -- Presione cualquier tecla para volver al menu.\r\n");
			LPSCI_HAL_SendDataPolling(UART0, (uint8_t *)Buffer,strlen(Buffer));
			FLAG_DATO_RX = false;
		}
}
void ADC0_IRQHandler(void)
{
	lecturaLuz();
}

void UART0_IRQHandler(void)
{
	LPSCI_HAL_ClearStatusFlag(UART0,kLpsciIntRxActiveEdge);
	LPSCI_HAL_Getchar(UART0, &caracterLeido);
	FLAG_DATO_RX = true;
}

void PIT_IRQHandler(void)
{
	PIT_HAL_ClearIntFlag(PIT, 1);
	/* manda intensidad luz cada 0.25seg si se apreto 'B'*/
	if(apretoB&&!detuvoTransmision)
	{
		sprintf(Buffer,"%d%c \r\n",luzPorcentaje,'%');
		LPSCI_HAL_SendDataPolling(UART0, (uint8_t *)Buffer,strlen(Buffer));
	}
}

/*==================[end of file]============================================*/
