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

void ToggleGreenLed(){
	GPIOD_PTOR |= 1<<5;
	}

void EnableGreenLedwithPWM(){
	
		SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; //habilito Puerto
		PORTD_PCR5 |=PORT_PCR_MUX(4); 		//selecciono Pin 5 Puerto D salida del PWM
		
}

void EnablePWM(){
	
	SIM_SCGC6 |= 1<<24; //CLK al TPM
	SIM_SOPT2 |= 1<<24; //Selecciono clk TPM MCGFLLCLK clock
	PORTD_PCR5 |=PORT_PCR_MUX(4); 		//selecciono Pin 5 Puerto D salida del PWM
	
	TPM0_MOD &= 0xFFFF;
	TPM0_SC |= 1<<3; //Clock Mode Selection incremento en flanco ascendente
	TPM0_C5SC |= 0x28;
	
}


void EnableADC(){
	
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK; //Doy clock al puerto E donde esta el pin del Sensor Light
	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;  // Doy Clock al ADC0 para encenderlo.
	
	ADC0_CFG1 |= ADC_CFG1_MODE(3); //Funciona en 16 bit
	ADC0_SC3  |= ADC_SC3_ADCO_MASK; //Fijo la conversion continua para que no frene nunca la conversión.
	
	NVIC_IPR3 |= 1<<31; //Fijo baja prioridad al ADCO; 
	NVIC_ISER |= 1<<15;	//Habilito Interrupciones ADC0 en el micro 
	
//	ADC0_SC1A &= ADC_SC1_ADCH(3); //Habilito el ADC0 dp3 canal del Sensor Light Segun the Schematic
//	ADC0_SC1A |= ADC_SC1_AIEN_MASK; //Habilito las interrupciones
	

}

void ADCOn(){
	ADC0_SC1A &= ADC_SC1_ADCH(3); //Habilito el ADC0 dp3 canal del Sensor Light Segun the Schematic
	ADC0_SC1A |= ADC_SC1_AIEN_MASK; //Habilito las interrupciones
	
}
 
void ADCOff(){
	ADC0_SC1A |= 1<<7;  //Bajo la bandera COCO del ADC0
	ADC0_SC1A |= ADC_SC1_ADCH(31); //Apago en conversor
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
void EnablePIT(){
	
			SIM_SCGC6 |= SIM_SCGC6_PIT_MASK; //Doy clock al PIT
			PIT_MCR &= ~PIT_MCR_MDIS_MASK; 
			PIT_LDVAL0 |= PIT_LDVAL_TSV(20000000*3);
	//		NVIC_ISER |= 1<<22; //habilitacion interrucion PIT en micro
	//		PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK //| PIT_TCTRL_TIE_MASK;
}
