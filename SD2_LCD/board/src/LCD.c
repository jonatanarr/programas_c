//#include "Cpu.h"
#include "MKL46Z4.h"
#include "LCD.h"
#include "LCDConfig.h"

/*used to indicate in which position of the LCD is the next character to write */
static uint8_t bLCD_CharPosition;
uint8_t lcd_alternate_mode;  //this variable must be 0 or 4
uint8_t bflgLCD_Scroll = 0x01;

extern const uint32_t *LCD_TO_PORT[];
extern const uint32_t MASK_BIT[32];
extern const uint8_t  WF_ORDERING_TABLE[];           //   Logical Front plane N to LCD_WFx

char   lcdBuffer[32]; // used for temporary storage for formating strings

/*
local functions
*/
void vfnLCD_Init(void);
void vfnEnablePins(void);  // Pin Enable/ BPEN / COM configurations
void vfnSetBackplanes(void); // reconfigure COM to default values
void vfnBP_VScroll(int8_t scroll_count);
void vfnLCD_Home (void);
void vfnLCD_Contrast (uint8_t lbContrast);
void vfnLCD_Write_MsgPlace (uint8_t *lbpMessage, uint8_t lbSize );
void vfnLCD_Write_Msg (uint8_t *lbpMessage);
void vfnLCD_All_Segments_ON (void);
void vfnLCD_All_Segments_Char (uint8_t val);
void vfnLCD_All_Segments_OFF (void);

void vfnLCD_Write_Char (uint8_t lbValue);  // This function is dependent on the LCD panel
void PutPoint(uint8_t x, uint8_t y);
void ClrPoint(uint8_t x, uint8_t y);
void SetX(uint8_t x, uint8_t value);


void lcd_pinmux(uint8_t  mux_val);

void vfnLCD_interrupt_init(void);
void vfnLCD_isrv(void);


void vfnLCD_Init(void)
{
    SIM_SCGC5 |= SIM_SCGC5_SLCD_MASK;
    /* LCD_GCR: LCDEN=0 */
    LCD_GCR &= (uint32_t)~(uint32_t)(LCD_GCR_LCDEN_MASK); /* Disable LCD device */
    /* PORTD_PCR0: ISF=0,MUX=0 */
    PORTD_PCR0 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    /* PORTE_PCR4: ISF=0,MUX=0 */
    PORTE_PCR4 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    /* PORTB_PCR23: ISF=0,MUX=0 */
    PORTB_PCR23 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    /* PORTB_PCR22: ISF=0,MUX=0 */
    PORTB_PCR22 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    /* PORTC_PCR17: ISF=0,MUX=0 */
    PORTC_PCR17 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    /* PORTB_PCR21: ISF=0,MUX=0 */
    PORTB_PCR21 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    /* PORTB_PCR7: ISF=0,MUX=0 */
    PORTB_PCR7 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    /* PORTB_PCR8: ISF=0,MUX=0 */
    PORTB_PCR8 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    /* PORTE_PCR5: ISF=0,MUX=0 */
    PORTE_PCR5 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    /* PORTC_PCR18: ISF=0,MUX=0 */
    PORTC_PCR18 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    /* PORTB_PCR10: ISF=0,MUX=0 */
    PORTB_PCR10 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    /* PORTB_PCR11: ISF=0,MUX=0 */
    PORTB_PCR11 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    /* LCD_GCR: RVEN=0,??=0,??=0,??=0,RVTRIM=0,CPSEL=1,??=0,LADJ=1,??=0,??=0,VSUPPLY=0,??=0,PADSAFE=0,FDCIEN=0,ALTDIV=0,ALTSOURCE=0,FFR=1,LCDDOZE=0,LCDSTP=0,LCDEN=0,SOURCE=1,LCLK=4,DUTY=3 */
    LCD_GCR = LCD_GCR_RVTRIM(0x00) |
                LCD_GCR_CPSEL_MASK |
                LCD_GCR_LADJ(0x01) |
                LCD_GCR_ALTDIV(0x00) |
                LCD_GCR_FFR_MASK |
                LCD_GCR_SOURCE_MASK |
                LCD_GCR_LCLK(0x04) |
                LCD_GCR_DUTY(0x03);        /* Set general control register */
    /* LCD_AR: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,BLINK=0,ALT=0,BLANK=0,??=0,BMODE=0,BRATE=0 */
    LCD_AR = (LCD_AR_BRATE(0x00) | 0x8000U); /* Set auxillary register */
    /* LCD_PENL: PEN=0x000E0D80 */
    LCD_PENL = LCD_PEN_PEN(0x000E0D80);  /* Set pin enable register low */
    /* LCD_PENH: PEN=0x00300160 */
    LCD_PENH = LCD_PEN_PEN(0x00300160);  /* Set pin enable register high */
    /* LCD_BPENL: BPEN=0x000C0000 */
    LCD_BPENL = LCD_BPEN_BPEN(0x000C0000); /* Set backplane enable register low */
    /* LCD_BPENH: BPEN=0x00100100 */
    LCD_BPENH = LCD_BPEN_BPEN(0x00100100); /* Set backplane enable register high */
    /* LCD_WF3TO0: WF3=0,WF2=0,WF1=0,WF0=0 */
    LCD_WF3TO0 = LCD_WF_WF3(0x00) |
                LCD_WF_WF2(0x00) |
                LCD_WF_WF1(0x00) |
                LCD_WF_WF0(0x00);       /* Clear waveform register */
    /* LCD_WF7TO4: WF7=0,WF6=0,WF5=0,WF4=0 */
    LCD_WF7TO4 = LCD_WF_WF7(0x00) |
            LCD_WF_WF6(0x00) |
            LCD_WF_WF5(0x00) |
            LCD_WF_WF4(0x00);       /* Clear waveform register */
    /* LCD_WF11TO8: WF11=0,WF10=0,WF9=0,WF8=0 */
    LCD_WF11TO8 = LCD_WF_WF11(0x00) |
            LCD_WF_WF10(0x00) |
            LCD_WF_WF9(0x00) |
            LCD_WF_WF8(0x00);      /* Clear waveform register */
    /* LCD_WF15TO12: WF15=0,WF14=0,WF13=0,WF12=0 */
    LCD_WF15TO12 = LCD_WF_WF15(0x00) |
            LCD_WF_WF14(0x00) |
            LCD_WF_WF13(0x00) |
            LCD_WF_WF12(0x00);    /* Clear waveform register */
    /* LCD_WF19TO16: WF19=0x44,WF18=0x88,WF17=0,WF16=0 */
    LCD_WF19TO16 = LCD_WF_WF19(0x44) |
            LCD_WF_WF18(0x88) |
            LCD_WF_WF17(0x00) |
            LCD_WF_WF16(0x00);    /* Configure which phase the backplane pins are active in. */
    /* LCD_WF23TO20: WF23=0,WF22=0,WF21=0,WF20=0 */
    LCD_WF23TO20 = LCD_WF_WF23(0x00) |
            LCD_WF_WF22(0x00) |
            LCD_WF_WF21(0x00) |
            LCD_WF_WF20(0x00);    /* Clear waveform register */
    /* LCD_WF27TO24: WF27=0,WF26=0,WF25=0,WF24=0 */
    LCD_WF27TO24 = LCD_WF_WF27(0x00) |
            LCD_WF_WF26(0x00) |
            LCD_WF_WF25(0x00) |
            LCD_WF_WF24(0x00);    /* Clear waveform register */
    /* LCD_WF31TO28: WF31=0,WF30=0,WF29=0,WF28=0 */
    LCD_WF31TO28 = LCD_WF_WF31(0x00) |
            LCD_WF_WF30(0x00) |
            LCD_WF_WF29(0x00) |
            LCD_WF_WF28(0x00);    /* Clear waveform register */
    /* LCD_WF35TO32: WF35=0,WF34=0,WF33=0,WF32=0 */
    LCD_WF35TO32 = LCD_WF_WF35(0x00) |
            LCD_WF_WF34(0x00) |
            LCD_WF_WF33(0x00) |
            LCD_WF_WF32(0x00);    /* Clear waveform register */
    /* LCD_WF39TO36: WF39=0,WF38=0,WF37=0,WF36=0 */
    LCD_WF39TO36 = LCD_WF_WF39(0x00) |
            LCD_WF_WF38(0x00) |
            LCD_WF_WF37(0x00) |
            LCD_WF_WF36(0x00);    /* Clear waveform register */
    /* LCD_WF43TO40: WF43=0,WF42=0,WF41=0,WF40=0x11 */
    LCD_WF43TO40 = LCD_WF_WF43(0x00) |
            LCD_WF_WF42(0x00) |
            LCD_WF_WF41(0x00) |
            LCD_WF_WF40(0x11);    /* Configure which phase the backplane pins are active in. */
    /* LCD_WF47TO44: WF47=0,WF46=0,WF45=0,WF44=0 */
    LCD_WF47TO44 = LCD_WF_WF47(0x00) |
            LCD_WF_WF46(0x00) |
            LCD_WF_WF45(0x00) |
            LCD_WF_WF44(0x00);    /* Clear waveform register */
    /* LCD_WF51TO48: WF51=0,WF50=0,WF49=0,WF48=0 */
    LCD_WF51TO48 = LCD_WF_WF51(0x00) |
            LCD_WF_WF50(0x00) |
            LCD_WF_WF49(0x00) |
            LCD_WF_WF48(0x00);    /* Clear waveform register */
    /* LCD_WF55TO52: WF55=0,WF54=0,WF53=0,WF52=0x22 */
    LCD_WF55TO52 = LCD_WF_WF55(0x00) |
            LCD_WF_WF54(0x00) |
            LCD_WF_WF53(0x00) |
            LCD_WF_WF52(0x22);    /* Configure which phase the backplane pins are active in. */
    /* LCD_WF59TO56: WF59=0,WF58=0,WF57=0,WF56=0 */
    LCD_WF59TO56 = LCD_WF_WF59(0x00) |
            LCD_WF_WF58(0x00) |
            LCD_WF_WF57(0x00) |
            LCD_WF_WF56(0x00);    /* Clear waveform register */
    /* LCD_WF63TO60: WF63=0,WF62=0,WF61=0,WF60=0 */
    LCD_WF63TO60 = LCD_WF_WF63(0x00) |
            LCD_WF_WF62(0x00) |
            LCD_WF_WF61(0x00) |
            LCD_WF_WF60(0x00);    /* Clear waveform register */


    ((((LCD_MemMapPtr)0x40053000u))->GCR) = (uint32_t)(((uint32_t)(((((LCD_MemMapPtr)0x40053000u))->GCR) & (uint32_t)(~(uint32_t)0x80u))) | ( \
    (uint32_t)((uint32_t)(1u) << 7)));
}


/* Enable pins according  WF_ORDERING_TABLE, BP_ORDERING_TABLE
 //Enable the LCD module frontplane waveform output (FP[39:0])
  LCD_PENH = 0xFFFFFFFF;
  LCD_PENL = 0xFFFFFFFF;

*/

  void vfnEnablePins (void)
  {
   uint8_t i;
   uint32_t *p_pen;
   uint8_t pen_offset;   // 0 or 1
   uint8_t pen_bit;      //0 to 31

   LCD_PENL = 0x0;
   LCD_PENH = 0x0;
   LCD_BPENL = 0x0;
   LCD_BPENH = 0x0;

   p_pen = (uint32_t *)&LCD_PENL;

    for (i=0;i<_LCDUSEDPINS;i++)
    {
      pen_offset = WF_ORDERING_TABLE[i]/32;
      pen_bit    = WF_ORDERING_TABLE[i]%32;
      p_pen[pen_offset] |= MASK_BIT[pen_bit];
      if (i>= _LCDFRONTPLANES)    // Pin is a backplane
      {
        p_pen[pen_offset+2] |= MASK_BIT[pen_bit];  // Enable  BPEN
        LCD_WF8B((uint8_t)WF_ORDERING_TABLE[i]) = MASK_BIT[i - _LCDFRONTPLANES];   // fill with 0x01, 0x02, etc
      }
    }
  }

/*
  Fill Backplanes with normal mask for  0x01, 0x02,  according to BP_ORDERING_TABLE;
*/
void vfnSetBackplanes(void)
 {
   uint8_t i;
   for (i=0;i<_LCDBACKPLANES;i++)
   {
       LCD_WF8B((uint8_t)WF_ORDERING_TABLE[i+_LCDFRONTPLANES]) = 0x01<<i;
   }
  }


 /*

 rotate Backplanes  scroll_y units
   if scroll_y>0  rotate to the right
   if scroll_y<0  rotate to the left
   if scroll_y=0  set normal configuration

*/
void vfnBP_VScroll(int8_t scroll_count)
 {
   uint8_t bp_count;
   if (scroll_count != 0)
   {
    for (bp_count=0;bp_count< _LCDBACKPLANES; bp_count++)
    {
       if (scroll_count>0)
         LCD_WF8B((uint8_t)WF_ORDERING_TABLE[_LCDFRONTPLANES +bp_count]) =  0x01<<(scroll_count+bp_count);
      else   //(scroll_count<0)
        {
           LCD_WF8B((uint8_t)WF_ORDERING_TABLE[_LCDFRONTPLANES +bp_count]) =   0x01>>(-scroll_count+bp_count);
        }
    }
   }
    else vfnSetBackplanes();
  }



void vfnLCD_Home (void)
 {
      bLCD_CharPosition =  0;
 }

void vfnLCD_Contrast (uint8_t lbContrast)
{
       lbContrast &= 0x0F;                                               //Forced to the only values accepted
       LCD_GCR |= LCD_GCR_RVTRIM(lbContrast);
}

 void  vfnLCD_Write_MsgPlace (uint8_t *lbpMessage, uint8_t lbSize )
 {

          if (lbSize > _CHARNUM)
          {
               vfnLCD_Scroll(lbpMessage, lbSize);
          }
          else
          {
            do
            {
                    vfnLCD_Write_Char (*lbpMessage);
                    lbpMessage ++;
             }while (--lbSize);
          }

 }



 /*
 vfnLCD_Write_Msg.  Display a Message starting at the first character of display
   until _CHARNUM  or End of string.
 */

void  vfnLCD_Write_Msg (uint8_t *lbpMessage) {
  uint8_t lbSize      = 0;
  bLCD_CharPosition = 0;  //Home display
  while (lbSize<_CHARNUM && *lbpMessage!='\0') {
    vfnLCD_Write_Char(*lbpMessage++);
    lbSize++;
  }
  if (lbSize<_CHARNUM) {
    while (lbSize++< _CHARNUM) {
      vfnLCD_Write_Char(BLANK_CHARACTER);  // complete data with blanks
    }
  }
}

void vfnLCD_All_Segments_ON (void)
{
 uint8_t lbTotalBytes = _CHARNUM * _LCDTYPE;
 uint8_t lbCounter=0;
 uint8_t *lbpLCDWF;

    lbpLCDWF = (uint8_t *)&LCD_WF3TO0;

        while (lbCounter < lbTotalBytes)
          {
              lbpLCDWF[(uint8_t)WF_ORDERING_TABLE[lbCounter++]]=_ALLON;
          }

}


void vfnLCD_All_Segments_Char (uint8_t val)
{
 uint8_t lbTotalBytes = _CHARNUM * _LCDTYPE;
 uint8_t lbCounter=0;
 uint8_t *lbpLCDWF;
    lbpLCDWF = (uint8_t *)&LCD_WF3TO0;
        while (lbCounter < lbTotalBytes)
          {
              lbpLCDWF[WF_ORDERING_TABLE[lbCounter++]]=val;
          }

}


void vfnLCD_All_Segments_OFF (void)
{
 uint8_t lbTotalBytes = _CHARNUM * _LCDTYPE;
 uint8_t lbCounter=0;
 uint8_t *lbpLCDWF;

    lbpLCDWF = (uint8_t *)&LCD_WF3TO0;
        while (lbCounter < lbTotalBytes)
          {
              lbpLCDWF[WF_ORDERING_TABLE[lbCounter++]]=0;
          }
}



/*

 Write a Char at position bLCD_CharPosition;
 After write is complete  bLCD_CharPosition point to next position

 Verify Char is a Valid character  between '0'  and 'z'
 in this case char is between 'a' to 'z' are taken as upper case

 If the char is a invalid char it is taken as ':'  code, in this case as SPACE

Aug-16   LP  arrayOffset was added due that array size with matrix symbols is greater than uint8

*/
#ifdef DOT_MATRIX

void vfnLCD_Write_Char (uint8_t lbValue)
{
          uint8_t char_val, temp;   //for test only
          uint8_t *lbpLCDWF;
          uint8_t lbCounter;

          uint16_t arrayOffset;
          uint8_t position;


          lbpLCDWF = (uint8_t *)&LCD_WF3TO0;


/*only ascci character if value not writeable write as @*/

          if (lbValue>='a' && lbValue<='z') lbValue -= 32; // UpperCase
          if (lbValue<ASCCI_TABLE_START || lbValue >ASCCI_TABLE_END) lbValue = BLANK_CHARACTER;  // default value as space

          lbValue -=ASCCI_TABLE_START;        // Remove the offset to search in the ascci table

          arrayOffset = (lbValue * _CHAR_SIZE); // Compensate matrix offset



// ensure bLCD position is in valid limit

          lbCounter =0;  //number of writings to complete one char
          while (lbCounter<_CHAR_SIZE  && bLCD_CharPosition < _CHARNUM )
          {
              //    position = (bLCD_CharPosition) *_CHAR_SIZE + lbCounter; //-1 to compensate first incomplete character
              //     position = (bLCD_CharPosition) *_LCDTYPE + lbCounter; //-1 to compensate first incomplete character
                     position = bLCD_CharPosition; // for DOt matrix


                    char_val = ASCII_TO_WF_CODIFICATION_TABLE[arrayOffset + lbCounter];
                    if (bLCD_CharPosition==0) //write complete character
                    {
                     lbpLCDWF[WF_ORDERING_TABLE[position]] = char_val;  //all 4 bits correspond to 7 digit character
                    }
                    else  //bit0 of second character is a symbol (DP o COL on the LCD) and must not be written
                    {

                      temp = lbpLCDWF[WF_ORDERING_TABLE[position]] & 0x01;  //bit 0 correspond to a Symbol
                      lbpLCDWF[WF_ORDERING_TABLE[position]] = char_val | temp;  //only bit 1,2,3 correspond to 7 digit character
                    }

                    if (char_val==0) lbCounter = _CHAR_SIZE; //end of this character
                    lbCounter++;
                    bLCD_CharPosition++;
          }

}

#else

// For 7 Segment LCD
void vfnLCD_Write_Char (uint8_t lbValue) {
  uint8_t char_val;
  uint8_t temp;
  uint8_t *lbpLCDWF;
  uint8_t lbCounter;
  uint16_t arrayOffset;
  uint8_t position;

  lbpLCDWF = (uint8_t *)&LCD_WF3TO0;
  /* only ascii character if value not writeable write as @ */
  if (lbValue>='a' && lbValue<='z') {
    lbValue -= 32; // UpperCase
  }
  if (lbValue=='-') {
      lbValue = '=';
  }
  if (lbValue<ASCCI_TABLE_START || lbValue >ASCCI_TABLE_END) {
    lbValue = BLANK_CHARACTER;  // default value as space
  }
  lbValue -=ASCCI_TABLE_START;        // Remove the offset to search in the ascci table
  arrayOffset = (lbValue * _CHAR_SIZE); // Compensate matrix offset
  // ensure bLCD position is in valid limit
  lbCounter = 0;  //number of writings to complete one char
  while (lbCounter<_CHAR_SIZE  && bLCD_CharPosition < _CHARNUM) {
    position = (bLCD_CharPosition) *_LCDTYPE + lbCounter;
    temp=0;
    if (lbCounter==1) {
      temp = lbpLCDWF[WF_ORDERING_TABLE[position]] & 0x01;//bit 0 has the special symbol information
    }
    char_val = ASCII_TO_WF_CODIFICATION_TABLE[arrayOffset + lbCounter];
    lbpLCDWF[WF_ORDERING_TABLE[position]] = char_val | temp;
    //  if (char_val==0) lbCounter = _CHAR_SIZE; //end of this character
    lbCounter++;
  }
  bLCD_CharPosition++;
}

#endif



void PutPoint(uint8_t x, uint8_t y)
 {
   uint8_t *lbpLCDWF;
   lbpLCDWF = (uint8_t *)&LCD_WF3TO0;

   if (x>= _CHARNUM  || y>7) return;
   lbpLCDWF[WF_ORDERING_TABLE[x]] |= (1<<y);

 }


void ClrPoint(uint8_t x, uint8_t y)
 {
   uint8_t *lbpLCDWF;
              lbpLCDWF = (uint8_t *)&LCD_WF3TO0;
              if (x>= _CHARNUM  || y>7) return;
              lbpLCDWF[WF_ORDERING_TABLE[x]] &= ~(1<<y);

 }



void SetX(uint8_t x, uint8_t value)
 {
   uint8_t *lbpLCDWF;
   lbpLCDWF = (uint8_t *)&LCD_WF3TO0;

              if (x>= _CHARNUM) return;
              lbpLCDWF[WF_ORDERING_TABLE[x]] = value;

 }



/*
 Configure LCD used pins as MUX=0  for LCD normal operation (analog operation)
 Configure LCD used pins as MUX=7  for LCD Fault detectionoperation (pull resistor  control enabled)
 only 0 or 7 is allowed for LCD operation
*/

void lcd_pinmux(uint8_t  mux_val)
{
   uint8_t i;

   for (i=0;i< _LCDUSEDPINS; i++)
   {
      *(uint32_t *)LCD_TO_PORT[WF_ORDERING_TABLE[i]] &= ~PORT_PCR_MUX_MASK; //Clear previos value
      *(uint32_t *)LCD_TO_PORT[WF_ORDERING_TABLE[i]] |= PORT_PCR_MUX(mux_val); // Set new value
   }
}

/*
  vfnLCD_interrupt_init
 for kinetis NVIC must be initialized in order to

*/
void vfnLCD_interrupt_init(void)
{

}

/*


*/
void vfnLCD_isrv(void)
 {
   LcdInterruptCallBack(); // User defined function
 }


/*
   This table relates LCD_Px to PORT Control Pin
   in used for pinmux function
*/


const uint32_t *LCD_TO_PORT[60] =
 {
    (uint32_t *) &PORTB_PCR0,  //LCD_P00
    (uint32_t *) &PORTB_PCR1,  //LCD_P01
    (uint32_t *) &PORTB_PCR2,  //LCD_P02
    (uint32_t *) &PORTB_PCR3,  //LCD_P03
    (uint32_t *) &PORTC_PCR20,  //LCD_P04
    (uint32_t *) &PORTC_PCR21,  //LCD_P05
    //(uint32_t *) &PORTC_PCR22,  //LCD_P06
    (uint32_t *) &PORTB_PCR0,  //LCD_P00  test

    (uint32_t *) &PORTB_PCR7,  //LCD_P07
    (uint32_t *) &PORTB_PCR8,  //LCD_P08
    (uint32_t *) &PORTB_PCR9,  //LCD_P09
    (uint32_t *) &PORTB_PCR10,  //LCD_P10
    (uint32_t *) &PORTB_PCR11,  //LCD_P11
    (uint32_t *) &PORTB_PCR16,  //LCD_P12
    (uint32_t *) &PORTB_PCR17,  //LCD_P13
    (uint32_t *) &PORTB_PCR18,  //LCD_P14
    (uint32_t *) &PORTB_PCR19,  //LCD_P15
    (uint32_t *) &PORTB_PCR20,  //LCD_P16
    (uint32_t *) &PORTB_PCR21,  //LCD_P17
    (uint32_t *) &PORTB_PCR22,  //LCD_P18
    (uint32_t *) &PORTB_PCR23,  //LCD_P19
    (uint32_t *) &PORTC_PCR0,  //LCD_P20
    (uint32_t *) &PORTC_PCR1,  //LCD_P21
    (uint32_t *) &PORTC_PCR2,  //LCD_P22
    (uint32_t *) &PORTC_PCR3,  //LCD_P23
    (uint32_t *) &PORTC_PCR4,  //LCD_P24
    (uint32_t *) &PORTC_PCR5,  //LCD_P25
    (uint32_t *) &PORTC_PCR6,  //LCD_P26
    (uint32_t *) &PORTC_PCR7,  //LCD_P27
    (uint32_t *) &PORTC_PCR8,  //LCD_P28
    (uint32_t *) &PORTC_PCR9,  //LCD_P29
    (uint32_t *) &PORTC_PCR10,  //LCD_P30
    (uint32_t *) &PORTC_PCR11,  //LCD_P31
    (uint32_t *) &PORTC_PCR12,  //LCD_P32
    (uint32_t *) &PORTC_PCR13,  //LCD_P33
    (uint32_t *) &PORTC_PCR16,  //LCD_P36
    (uint32_t *) &PORTC_PCR17,  //LCD_P37
    (uint32_t *) &PORTC_PCR18,  //LCD_P38
    (uint32_t *) &PORTC_PCR23,  //LCD_P39
    (uint32_t *) &PORTD_PCR0,  //LCD_P40
    (uint32_t *) &PORTD_PCR1,  //LCD_P41
    (uint32_t *) &PORTD_PCR2,  //LCD_P42
    (uint32_t *) &PORTD_PCR3,  //LCD_P43
    (uint32_t *) &PORTD_PCR4,  //LCD_P44
    (uint32_t *) &PORTD_PCR5,  //LCD_P45
    (uint32_t *) &PORTD_PCR6,  //LCD_P46
    (uint32_t *) &PORTD_PCR7,  //LCD_P47
    (uint32_t *) &PORTE_PCR0,  //LCD_P48
    (uint32_t *) &PORTE_PCR1,  //LCD_P49
    (uint32_t *) &PORTE_PCR2,  //LCD_P50
    (uint32_t *) &PORTE_PCR3,  //LCD_P51
    (uint32_t *) &PORTE_PCR4,  //LCD_P52
    (uint32_t *) &PORTE_PCR5,  //LCD_P53
    (uint32_t *) &PORTE_PCR6,  //LCD_P54
    (uint32_t *) &PORTE_PCR16,  //LCD_P55
    (uint32_t *) &PORTE_PCR17,  //LCD_P56
    (uint32_t *) &PORTE_PCR18,  //LCD_P57
    (uint32_t *) &PORTE_PCR19,  //LCD_P58
    (uint32_t *) &PORTE_PCR20,  //LCD_P59
    (uint32_t *) &PORTE_PCR21,  //LCD_P60


    };

const uint32_t MASK_BIT[32] =
{
       0x00000001 ,
       0x00000002 ,
       0x00000004 ,
       0x00000008 ,
       0x00000010 ,
       0x00000020 ,
       0x00000040 ,
       0x00000080 ,
       0x00000100 ,
       0x00000200 ,
       0x00000400 ,
       0x00000800 ,
       0x00001000 ,
       0x00002000 ,
       0x00004000 ,
       0x00008000 ,
       0x00010000 ,
       0x00020000 ,
       0x00040000 ,
       0x00080000 ,
       0x00100000 ,
       0x00200000 ,
       0x00400000 ,
       0x00800000 ,
       0x01000000 ,
       0x02000000 ,
       0x04000000 ,
       0x08000000 ,
       0x10000000 ,
       0x20000000 ,
       0x40000000 ,
       0x80000000 ,
};

