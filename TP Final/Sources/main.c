
#include "derivative.h" /* include peripheral declarations */
#include "Macros.h"



int main(void)
{
	_Bool L[11]={1,0,0,0,0,0,0,0,0,0,0};
	_Bool T[12]={0,0,0,0,0,0,0,0,0,0,0,0};
	_Bool Ct=0, S=0, FT=0;		
	_Bool P1=0,P2=0;
		
			EnableSw2();
			EnableSw1();
			EnablePIT();
			EnableADC();
			EnableGreenLedwithPWM();
			EnablePWM();
			

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
			
			T[6]=P2&&!P1&&L[0];
			T[7]=FT&&L[6];
			T[8]=!FT&&L[7];
			T[9]=P1&&!P2&&L[8];
			T[10]=FT&&L[9];
			T[11]=!FT&&L[10];
			

			
			
			
			if(T[0]){ L[0]=0; L[1]=1;}
			if(T[1]){ L[1]=0; L[2]=1;}
			if(T[2]){ L[2]=0; L[3]=1;}
			if(T[3]){ L[3]=0; L[4]=1;}
			if(T[4]){ L[4]=0; L[5]=1;}
			if(T[5]){ L[5]=0; L[0]=1;}
			
			if(T[6]){ L[0]=0; L[6]=1;}
			if(T[7]){ L[6]=0; L[7]=1;}
			if(T[8]){ L[7]=0; L[8]=1;}
			if(T[9]){ L[8]=0; L[9]=1;}
			if(T[10]){ L[9]=0; L[10]=1;}
			if(T[11]){ L[10]=0; L[0]=1;}
		
			
	S= L[3] || L[4] || L[8] || L[9];
	Ct=L[1] || L[4] || L[6] || L[9];
	
	

	if(Ct==1){
		SetPIT(); }
	else{
		ClearPIT();
	}
	
	if(S==1){
		ADCOn();
	}
		else{
		ADCOff();	
		TPM0_C5V = 0xFFFF;
				
	}
	
		}
	
	return 0;
}


void ADC0_IRQHandler(){
	
	ADC0_SC1A |= 1<<7;  //Bajo la bandera COCO del ADC0
	
	
	TPM0_C5V = ADC0_RA;

	
}


	
