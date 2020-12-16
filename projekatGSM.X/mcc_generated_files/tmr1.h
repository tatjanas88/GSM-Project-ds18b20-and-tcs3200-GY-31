/**
  TMR1 Generated Driver API Header File 

  @Company
    Microchip Technology Inc.

  @File Name
    tmr1.h

  @Summary
    This is the generated header file for the TMR1 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This header file provides APIs for driver for TMR1. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.169.0
        Device            :  PIC24FJ256GA106
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.50
        MPLAB 	          :  MPLAB X v5.40
*/


#ifndef _TMR1_H
#define _TMR1_H

/**
  Section: Included Files
*/

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "clock.h"

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

#define TMR1_INTERRUPT_TICKER_FACTOR    1


void TMR1_Initialize (void);

void __attribute__ ( ( interrupt, no_auto_psv ) ) _T1Interrupt (  );




#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif //_TMR1_H
    
/**
 End of File
*/
