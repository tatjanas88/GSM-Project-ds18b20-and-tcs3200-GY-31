
/**
  TMR1 Generated Driver API Source File 

  @Company
    Microchip Technology Inc.

  @File Name
    tmr1.c

  @Summary
    This is the generated source file for the TMR1 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for driver for TMR1. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.169.0
        Device            :  PIC24FJ256GA106
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.50
        MPLAB             :  MPLAB X v5.40
*/



/**
  Section: Included Files
*/

#include <stdio.h>
#include "tmr1.h"
#include"uart1.h"

/**
 Section: File specific functions
*/
#define FCY 16000000 





//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

/**
  Section: Driver Interface
*/

void TMR1_Initialize (void)
{
    T1CONbits.TON = 0;
    //INTERRUPT FLAG STATUS
    IFS0bits.T1IF = 0;
    
     //PODESAVANJE TAJMERA
    T1CONbits.TSIDL = 1;
    T1CONbits.TCKPS = 0;
    T1CONbits.TSYNC = 0;
    T1CONbits.TGATE = 0;
    T1CONbits.TCS = 0;
    //PODESAVANJE PRIORITY INTERRUPT
    IPC0bits.IC1IP = 1; //HIGHEST PRIORITY
    //INTERRUPT ENABLE 
    IEC0bits.T1IE = 1; //za tajmer1
    
    PR1 = ((double)FCY/1600);
    TMR1 = 0;
    
    T1CONbits.TON = 1; //SET TIMER ON
    

}





/**
 End of File
*/
