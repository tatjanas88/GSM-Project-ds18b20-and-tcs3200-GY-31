
#include <string.h>
#include <stdio.h>
#include "color.h"

char Rvalue[10];
char Bvalue[10];
char Gvalue[10];

unsigned int Rmin = 182;
unsigned Rmax = 1212;
unsigned Gmin  = 182;
unsigned Gmax = 1196;
unsigned Bmin = 181;
unsigned Bmax = 1194;

unsigned long r,g,b,clear;
unsigned int R,G,B;
//moze se staviti uint16 ya frekvenciju
/*char valueR[10];
char valueB[10];
char valueG[10];
 */ 

unsigned int overflow_count=0;
unsigned int period = 0;

//ColorSensor
char RedValue = 0;
char BlueValue = 0;
char GreenValue = 0;

#define S0 LATBbits.LATB8
#define S1 LATBbits.LATB9
#define S2 LATBbits.LATB10
#define S3 LATBbits.LATB11

#define LED TRISBbits.TRISB13

#define INPUT PORTBbits.RB12



void TCS3200_Config(void)
{
    //outputs
    AD1PCFGL = 0xFFFF; //da su GPIO digitalni/provereno
    TRISBbits.TRISB8 = 0; //S0
    TRISBbits.TRISB9 = 0;//S1
    TRISBbits.TRISB10 = 0; //S2
    TRISBbits.TRISB11 = 0; //S3
    TRISBbits.TRISB13 = 0; //LED //preporucuje se da sve sijaju
    
    TRISBbits.TRISB12 = 1; // OUT je input
    
    LATBbits.LATB8 = 0;
    LATBbits.LATB9 = 0;
    LATBbits.LATB10 = 0;
    LATBbits.LATB11 = 0;
    
    //LATBbits.LATB12 = 0;
    
    TRISCbits.TRISC13=0;     //led check
    TRISCbits.TRISC14 = 0;
            
    LATCbits.LATC13=0;//LD1
    PORTCbits.RC13 = 0;
    LATCbits.LATC14=0; //LD2
    PORTCbits.RC14 = 0;
    config_iocb();
}

/*void __attribute__ ( ( interrupt, no_auto_psv ) ) _T2Interrupt (  )
{
    TMR2 = 0;
    if(overflow_count == 0) //prvi put kad udje u interrupt onda je 0, vec drugi put odnosno druga ivica bice na 2
     T2CONbits.TON = 1;
    //INTERRUPT FLAG STATUS
    overflow_count++;
    
    if(overflow_count == 2) 
    {
        T2CONbits.TON = 0;
        overflow_count = 0; //gotova perioda i vratimo na 0
        period = TMR2;
    }
    IFS0bits.T2IF = 0;
}
 */ 
//TMR2 Config

unsigned int edge_counter = 0;
unsigned int cn_flag = 0;

void __attribute__((__interrupt__, no_auto_psv)) _CNInterrupt(void)
{
    _CNIF = 0; //svaki interrupt resetuje se fleg
     
    edge_counter++;
    //if(edge_counter == 100) //ovo se moze povecavati ya duzi period blikanja
    //{
         //LATCbits.LATC13 = ~ LATCbits.LATC13; //blink led1
        //LATCbits.LATC13 = 1;
       
        //LATCbits.LATC13 = 0;
        //edge_counter = 0;
    //}
    if(edge_counter == 1)
    {
        TMR2 = 0;
        T2CONbits.TON = 1; //start tajmer
    }
    if(edge_counter ==3)
    {
        LATCbits.LATC13 = 1;
        T2CONbits.TON = 0; //stop t2
        edge_counter = 0;
        period = TMR2;
        //period = 1000;
        cn_flag = 1;
    }
    
    
}

void config_iocb()
{
    IEC1bits.CNIE = 1;
    CNEN2bits.CN30IE = 0;
    IPC4bits.CNIP = 0x7; //highest priority
    TRISBbits.TRISB12 = 1; // OUT je input
    _CNIF = 0;    
}
void enable_cn()
{
    CNEN2bits.CN30IE = 1;
    _CNIF = 0; 
}
void disable_cn()
{
     CNEN2bits.CN30IE = 0;
    _CNIF = 0; 
}
void TMR2_init(void)
{
   
    T2CONbits.TON = 0;
    T2CONbits.TSIDL = 0;
    //T2CONbits.TCKPS = 0b10;
    T2CONbits.TCKPS = 0b01;
    T2CONbits.T32 = 0; //da je 16bitni   
    T2CONbits.TCS = 0;
    T2CONbits.TGATE = 0;
     
    
    //IEC0bits.T2IE = 1;
    //IFS0bits.T2IF = 0;
    //PR2 = ((double)FCY/1600);
	TMR2 = 0;
    //PORTCbits.RC14 = 1; //LD2
    
    //IPC4bits.CNIP = 0x7; 

}


unsigned long Frequency_Count()
{
    
    unsigned int count;
    //unsigned int val;
    //char value[10];       
    TMR2 = 0;
    /*while(INPUT == 1);
    while(INPUT == 0);
    while(INPUT == 1);
    T2CONbits.TON = 1;             /* turn ON timer */

    /*while(INPUT == 0);                  
    T2CONbits.TON = 0;            /* turn OFF timer */
    /*count = TMR2;
    if(count!=0)
    {
        PORTCbits.RC13 = 1;
    }
    //count = 10*(count + ((65536*overflow_count)));  /* if timer overflows then add that overflow count */
    //overflow_count=0;
    enable_cn();
    while(cn_flag == 0){
    }
    cn_flag = 0;  
   
    count = period;
    return count; 
   
}
// 256*256*256=16777216 possible colors.

unsigned int Map_RGB(unsigned int max, unsigned int min,unsigned int count)
{
    unsigned int val;
    val = 255*(float)((float)(count - min)/(float)(max-min));
    return val;
}


 /*unsigned int Map_RGB(unsigned int count, unsigned int min,unsigned int max, unsigned int out_min,unsigned int out_max)
{
    unsigned int val;
    val = (((count - min)*(out_max - out_min))/(max - min)) + out_min;
    return val;
}
  */
  

void Set_Filter (uint8_t mode) //select which photodiodes are active
{
    switch(mode)
    {
        case(Red):
            S2 = 0;
            S3 = 0; 
            break;
        case (Blue):
            S2 = 0;
            S3 = 1;
            break;
        case (Clear):
            S2 = 1;
            S3 = 0;
            break;
        case (Green):
            S2 = 1;
            S3 = 1;
            break;
            
    }
}
void Set_Scaling (uint8_t mode)
{
    switch(mode)
    {
        case (Scl0): //0%
            S0 = 0;
            S1 = 0;
            break;
        case (Scl2): //2%
            S0 = 0;
            S1 = 1;
            break;
        case (Scl20): //20 %
            S0 = 1;
            S1 = 0;
            break;
        case (Scl100): //100%
            S0 = 1;
            S1 = 1;
    }
}
/*unsigned char Take_Reading(void)
{
    T2CONbits.TON = 0;
    // Wait for the falling edge
    //while(INPUT);
    
    // Wait for the rising edge
    //while(!INPUT);
    
    // Now wait for the falling edge
    while(INPUT);
    
    // Now that we are sure of where we are, start counting!
    TMR2 = 0;
    T2CONbits.TON = 1;
    
    PORTCbits.RC13 = 1; 
   
       
    // Wait for the rising edge
    while(!INPUT);
    T2CONbits.TON = 0;
      
    PORTCbits.RC14 = 1;
    
    return TMR2;
            
}
 */ 

unsigned int Measure_Red(void)
{
    disable_cn();
    //PORTCbits.RC14 = 0;
        
    Set_Filter(Red);
    __delay_ms(100);
    //r = Frequency_Count();
    unsigned int red = Frequency_Count();
    //R = Map_RGB(Rmax, Rmin, red);
    //PORTCbits.RC14 = 1;
    /*if(R>255)
    {
      R = 255;
    }
     */ 
    return red;
    //sprintf(valueR," %d   ",red);
    
}
unsigned int Measure_Blue(void)
{
    disable_cn();
    
    Set_Filter(Blue);
     __delay_ms(100);
    //b = Frequency_Count();          /* find Frequency of Green color for given object */
    unsigned int blue = Frequency_Count();
    //B = Map_RGB(Bmax, Bmin, blue) - 40;      /* map frequency to RGB in range 0-255 */
    /*if(B>255)
    {
      B = 255;
    }
     */ 
    
    return blue;
    //sprintf(valueB," %d   ",blue);
 
}
unsigned int Measure_Green(void)
{
    disable_cn();
     
    Set_Filter(Green);
     __delay_ms(100);
    //g = Frequency_Count();          /* find Frequency of Green color for given object */
    unsigned int green = Frequency_Count();
    //G = Map_RGB(Gmax, Gmin, green) + 30;      /* map frequency to RGB in range 0-255 */
    /*if(G>255)
    {
      G = 255;
    }
     */ 
    return green;
    //sprintf(valueG," %d   ",green);
    
}
