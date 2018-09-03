


#include "derivative.h" /* include peripheral declarations */
#include "MisMacros.h"


int main(void)
{
	EnableSw1();
	EnableSw2();
	//EnableGreenLed();
	EnableADCO();
	EnablePIT();
	EnableGreenLedwithPIT();
	EnableTPMasPWM();
	
	NVIC_ISER |= 1<<15;	//Habilito Interrupciones ADC0 en el micro 
	
		_Bool L[11]={1,0,0,0,0,0,0,0,0,0,0};
		_Bool T[12]={0,0,0,0,0,0,0,0,0,0,0,0};
	
		_Bool P1=0,P2=0,FT =0; 
		_Bool ct=0, S=0;
	
	
	for(;;) {
		
		P1=sw1();
		P2=sw2();
		FT= PIT_TFLG0 && PIT_TFLG_TIF_MASK;
		
						
		
		T[0]=!P2&&P1&&L[0];
		T[1]=FT&&L[1];
		T[2]=!FT&&L[2];
		T[3]=!P1&&P2&&L[3];
		T[4]=FT&&L[4];
		T[5]=!FT&&L[5];
		
		
		if(T[0]){
			L[0]=0;
			L[1]=1;
		}
		if(T[1]){
			L[1]=0;
			L[2]=1;
		}
		if(T[2]){
			L[2]=0;
			L[3]=1;
		}
		if(T[3]){
			L[3]=0;
			L[4]=1;
		}
		if(T[4]){
			L[4]=0;
			L[5]=1;
		}
		if(T[5]){
			L[5]=0;
			L[0]=1;
		}
	
		
	
		if(L[3] | L[4]){
				S=1;
						}
		else{
				S=0; 
		}
		if(L[1] | L[4]){
			ct=1;
		}
		else{
			ct=0;
		}
		
		if(S==1){
			GreenLedOnPWM();
					
			}
		else{
			GreenLedOffPWM();
		}
		
		if(ct==1){
			SetPIT();
					
			}
		else{
			ClearPIT();
		}		
			
		

	
}
	
	
	return 0;
}


void ADC0_IRQHandler(){
	
	ADC0_SC1A |= 1<<7; //limpio la bandera COCO
	
	TPM0_SC &= ~(1<<3);
	TPM0_C5V = ADC0_RA;
	ADC0_RA;
	TPM0_SC |= 1<<3;
	
}
