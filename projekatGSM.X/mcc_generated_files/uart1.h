/**
  UART1 Generated Driver API Header File 

  @Company
    Microchip Technology Inc.

  @File Name
    uart1.h

  @Summary
    This is the generated header file for the UART1 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This header file provides APIs for driver for UART1. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.169.0
        Device            :  PIC24FJ256GA106
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.50
        MPLAB             :  MPLAB X v5.40
*/


#ifndef _UART1_H
#define _UART1_H

/**
 Section: Included Files
*/

#include <stdbool.h>
#include <stdint.h>
#include "string.h"
#include "clock.h"


#define BaudRateU1 9600
#define RXbuffLen1 100

#define CONNECTION_CHECK 0
#define SMS_FORMAT_DEFINE 1
#define SMS_LIST 2
#define SMS_READ 3
#define SEARCH_PHONEBOOK 4
#define SMS_SEND 5
#define SMS_DELETE 6

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
        
//vrlo vazno, ovdje definisati extern promjenjive, da bi se mole koristiti u mainu
//-----------------UART1-------------------------
extern char receivedBuffer[RXbuffLen1];

//-----------------TIMER1------------------------
extern unsigned int timerHandler;
extern unsigned char timerElapsed;
//-----------------MODEM-------------------------
extern unsigned int state;
//-----------------------------------------------


void UART1_Init(void);
void Send_One_Char(unsigned char data);
void Send_String_UART1(char *str);

void ClearBuffer(void);
unsigned char ParseBuffer(char * bufferWhere,const char * bufferWhat);

void BACK_TO_CHECK_CONNECTION_STATE(void);

void MODEM_CHECK_CONNECTION(void);
void MODEM_SMS_FORMAT_DEFINE(void);
void MODEM_SMS_LIST(void);
void MODEM_SMS_READ(void);
void MODEM_SEARCH_PHONEBOOK(void);
void MODEM_SMS_SEND(char *color, char *temperature);
void MODEM_SMS_DELETE(void);


#ifdef __cplusplus  // Provide C++ Compatibility

    

#endif
    
#endif  // _UART1_H