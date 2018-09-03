
//Freedom Freescale FRDM KL46Z256
//creador por Federico Ceccarelli para la Asignatura Sistemas Digitales I
//Universidad Nacional de Rosario.


void EnablePIT(){
	
			SIM_SCGC6 |= SIM_SCGC6_PIT_MASK; //Doy clock al PIT
			PIT_MCR &= ~PIT_MCR_MDIS_MASK; 
			PIT_LDVAL0 |= PIT_LDVAL_TSV(20000000*3);
	//		NVIC_ISER |= 1<<22; //habilitacion interrucion PIT en micro
	//		PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK //| PIT_TCTRL_TIE_MASK;
}

void EnableADCO(){ 
	
		SIM_SCGC6 |= 1<<27; //clock adc
		ADC0_CFG1 |= ADC_CFG1_MODE(3); //16 bit conversion  y bus clock
		ADC0_SC3  |= ADC_SC3_ADCO_MASK; //Fijo la conversion continua para que no frene nunca la conversión.
		ADC0_SC1A &= ADC_SC1_ADCH(3); // selecciona el canal 3
		ADC0_SC1A |= ADC_SC1_AIEN_MASK; //Habilito las interrupciones (No se por que si cambio el orden con el paso anterior
										//se me baja AIEN
		NVIC_IPR3 |= 1<<31; 
		//NVIC_ISER |= 1<<15;	//Habilito Interrupciones ADC0 en el micro 

}

void EnableGreenLedwithPIT(){
	
		SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; //habilito Puerto
		PORTD_PCR5 |=PORT_PCR_MUX(4); 		//selecciono Pin 5 Puerto D salida del PWM
		
}

void EnableTPMasPWM(){

SIM_SCGC6 |= 1<<24; //CLK al TPM
SIM_SOPT2 |= 1<<24; //Selecciono clk TPM MCGFLLCLK clock
TPM0_MOD &= 0xFFFF; 
TPM0_SC |= 1<<3; //Clock Mode Selection incremento en flanco ascendente
TPM0_C5SC |= 0x28; //Seleciono el modo pwm edge-aligned
TPM0_C5V =0xEFFF;
}

void EnableSw1(){

	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC_PCR3 |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	GPIOC_PDDR &=  ~(1<<3);
}

int sw1(){
	return !(GPIOC_PDIR & 1<<3);
}

void EnableSw2(){

	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC_PCR12 |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	GPIOC_PDDR &=  ~(1<<12);
}

int sw2(){
	return !(GPIOC_PDIR & 1<<12);
}



void EnableGreenLed(){

	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD_PCR5 |= PORT_PCR_MUX(1);
	GPIOD_PDDR |= 1<<5;
	GPIOD_PSOR |= (1<<5);
}

void GreenLedOn(){
	GPIOD_PCOR |= 1<<5;
	//while( (ADC0_SC1A & 0x80) == 0){}	//espera por coco

		//TPM0_C5V=ADC0_RA;
	
}

void GreenLedOnPWM(){ 		
	


TPM0_SC &= 0<<3;
TPM0_C5V = ADC0_RA;
ADC0_RA;
TPM0_SC |= 1<<3;

}

void GreenLedOffPWM(){
	TPM0_C5V=0XFFFF;
	
	}
	

void GreenLedOff(){
	GPIOD_PSOR |= 1<<5;
}

void EnableRedLed(){

	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE_PCR29 |= PORT_PCR_MUX(1);
	GPIOE_PDDR |= 1<<29;
	GPIOE_PSOR &= ~(1<<29);
}

void RedLedOn(){
	GPIOE_PDOR |= 1<<29;
}
void RedLedOff(){
	GPIOE_PDOR &= ~(1<<29);
}

void ToggleRedLed(){
	GPIOE_PTOR |= 1<<29;
	}
void ToggleGreenLed(){
	GPIOD_PTOR |= 1<<5;
	}

void SetPIT(){
	
	PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK;
	PIT_LDVAL0 |= PIT_LDVAL_TSV(20000000*3);
}

void ClearPIT(){
	
	PIT_TFLG0 |= 1<<0;
	PIT_TCTRL0 &= ~PIT_TCTRL_TEN_MASK;
	PIT_LDVAL0 |= PIT_LDVAL_TSV(20000000*3);

}

void EnableSysTick(){
	
	SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(3);  
	MCG_C4 |=  MCG_C4_DMX32_MASK ;
	SYST_RVR = 0x5B8000;
	SYST_CSR |= SysTick_CSR_CLKSOURCE_MASK | SysTick_CSR_TICKINT_MASK ; 	
	//SYST_CSR |= SysTick_CSR_ENABLE_MASK;
}




