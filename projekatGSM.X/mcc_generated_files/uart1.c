



/**
  Section: Included Files
*/
#include "xc.h"
#include "uart1.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "../ds18b20_sensor.h"


//-----------------------------------------------
unsigned int temp;
char message_index;
unsigned int temp_phone, temp_index;
unsigned char number, index;
unsigned char phone_number[16], index_number[100];
//-----------------MODEM-------------------------
unsigned int state;
//-----------------------------------------------
// timer interrupt
unsigned int timerHandler;
unsigned char timerElapsed;
//-----------------UART1-------------------------
char receivedBuffer[RXbuffLen1];
//-----------------TEMPSENOR---------------------
unsigned char temp_char[] = "000.0000 C";

char temp_string[6];
/** UART Driver Queue

  @Summary
    Defines the Transmit and Receive Buffers

*/



/**
  Section: Driver Interface
*/



void UART1_Initialize(void)
{
    IEC0bits.U1TXIE = 0;
    IEC0bits.U1RXIE = 0;

    U1MODEbits.ABAUD = 0;
    U1MODEbits.BRGH = 0;
    U1MODEbits.IREN = 0;
    U1MODEbits.LPBACK = 0;
    U1MODEbits.PDSEL = 0;
    U1MODEbits.PDSEL0 = 0;
    U1MODEbits.PDSEL1 = 0;
    U1MODEbits.RTSMD = 1; //simpplex mode
    U1MODEbits.RXINV = 0;
    U1MODEbits.STSEL = 0;
    U1MODEbits.USIDL = 0;
    U1MODEbits.WAKE = 0;
    U1MODEbits.UEN = 0;
    U1MODEbits.UEN0 = 0;
    U1MODEbits.UEN1 = 0;
    U1BRG = (FCY/((double)16 * BaudRateU1))-1;   //9600 na 16MHz
    
    //STATUS AND CONTROL REGISTAR
    U1STAbits.ADDEN = 0;
    U1STAbits.UTXISEL1=0;   //bit15 interrupt when char is transfered
    U1STAbits.UTXISEL0=0;   //bit13
    U1STAbits.UTXINV=0;     //bit14 no ir encoder transmit polarity inversion
    U1STAbits.UTXBRK=0;     //bit11 no transmit break bit
    U1STAbits.UTXEN=1;      //bit10 TX pin controled by peripheral
    U1STAbits.UTXBF=0;      //bit9 transmit buffer is not full,at least one more characte can be written
    U1STAbits.TRMT=0;       //bit8 transmit register is not empty,transmission is in progress
    U1STAbits.URXISEL1=0;   //bit7 receive interrupt mode selection bits
    U1STAbits.URXISEL0=0;   //bit6
    U1STAbits.ADDEN=0;      //bit5 address detect mode disabled
    U1STAbits.RIDLE=0;      //bit4 receiver is active idle bit read only
    U1STAbits.PERR=0;       //bit3 parity error status bit has not been detected
    U1STAbits.FERR=0;       //bit2 framing error has not been detected
    U1STAbits.OERR=0;       //bit1 receive buffer has not overflowed
    U1STAbits.URXDA=0;
    
    //INTERRUPT PRIORITY
    IPC2bits.U1RXIP = 1; //rx je na IPC2
    IPC3bits.U1TXIP = 0; //tx na IPC3
    //CLEAR FLAGS
    IFS0bits.U1RXIF = 0;
    IFS0bits.U1TXIF = 0;

    //interrupt enable and disable
    IEC0bits.U1RXIE = 1;
    IEC0bits.U1TXIE = 0; //nema interapta za slanje od kontrolera
    
    //Make sure to set LAT bit corresponding to TxPin as high before UART initialization
    U1MODEbits.UARTEN = 1;   // enabling UART ON bit
    U1STAbits.UTXEN = 1;
    
    //unlock register
    __builtin_write_OSCCONL(OSCCON & 0xbf);
    //za ulaz se koristi RPIN, a izlaz RPOR (SAMO SE NADJE UART1)   
      RPINR18bits.U1RXR=4;//PIN 43=RD9=RP4 RX za zelenu UART1 //bilo koji od 5:0
      RPOR1bits.RP3R=3;//PIN 44=RD10=RP3 TX za zelenu UART1 //a koji tacno RP def se u .RPxR.. mora 3, tako pise u tabeli za izlaze
     //lock register 
    __builtin_write_OSCCONL(OSCCON | 0x40); //START OSC.
    

//-----------UART1------------------------        
      TRISDbits.TRISD9=1; //RX UART *(GSM) ovo je ulazni pin rx
      TRISDbits.TRISD10=0; //TX UART   izlazni pin tx
}

/**
    Maintains the driver's transmitter state machine and implements its ISR
*/

void Send_One_Char(unsigned char data)
{
    while (!U1STAbits.TRMT); //wait for TXreg to become empty
    U1TXREG=data;
}
void Send_String_UART1(char *str)
{
    while(*str)
    {
        Send_One_Char(*str);
		str++;
    }
}
void ClearBuffer(void)
{
    int j;
    
    for (j=0; j < RXbuffLen1; j++)
	{
        receivedBuffer[j]='0';
	}
}

unsigned char ParseBuffer(char * bufferWhere,const char * bufferWhat)
{
       unsigned int res1;
       char *ptr1;
       ptr1 = strstr(bufferWhere, bufferWhat);
    
       if (ptr1 != NULL) 
       {
            res1 = ptr1 - bufferWhere + 1;
       }      
       else 
       {
            res1 = 0;
       }      
       return res1;
}
void BACK_TO_CHECK_CONNECTION_STATE(void)
{
        if (timerElapsed)
        {
            timerElapsed=0;
            state=CONNECTION_CHECK;
        }
          
        ClearBuffer();
        __delay_ms(500);
}
void MODEM_CHECK_CONNECTION(void)
{
        Send_String_UART1("AT\r");   
        
        timerHandler=5000;
        //timerHandler=2000;
        timerElapsed=0;
        temp=0;
   
        while((ParseBuffer(receivedBuffer,"OK")==0)&&(timerElapsed==0)) ;///CEKA SE ODGOVOR OK
        
        temp=ParseBuffer(receivedBuffer,"OK");//VRACA INDEKS(REDNO MESTO) U KOM DELU NIZA SE NALAZI "OK"

        if(temp!=0)
        {  
            state = SMS_FORMAT_DEFINE;              
        }
                        
        BACK_TO_CHECK_CONNECTION_STATE();
        
}
void MODEM_SMS_FORMAT_DEFINE(void)
{
        Send_String_UART1("AT+CMGF=1\r"); //format as text
         
        timerHandler=3000;
        timerElapsed=0;
        temp=0;
   
        while((ParseBuffer(receivedBuffer,"OK")==0)&&(timerElapsed==0)) ;
        
        temp=ParseBuffer(receivedBuffer,"OK");

        if(temp!=0)
        {  
            state = SMS_LIST;     
        }
       
        BACK_TO_CHECK_CONNECTION_STATE();
}
void MODEM_SMS_LIST(void)
{
        Send_String_UART1("AT+CMGL=ALL\r"); 
   
        timerHandler=5000;
        timerElapsed=0;
        temp=0;
        temp_index =0;
        //ovdje TODO: mijenjaj unread u read
        while((ParseBuffer(receivedBuffer,"REC UNREAD")==0)&&(timerElapsed==0));
        
        temp=ParseBuffer(receivedBuffer,"REC UNREAD");

        message_index=receivedBuffer[temp-4]; //od prvog pojavljivanja "REC UNREAD" oduzme 4 pozicije i dobije index 
        
        /*if(temp!=0)
        {    
            temp_index=ParseBuffer(receivedBuffer,":"); //trazi :
            state = SMS_READ;               
        }
                
        for(index=0; index < 100; index++)
        {
            index_number[index]=0;
        }
                
        for(index=0; receivedBuffer[temp_index+index] != ','; index++) //trazi sve do narednog zareza
        {
            index_number[index] = receivedBuffer[temp_index+index];
        }
         */ 
             
        state = SMS_READ;
        
        BACK_TO_CHECK_CONNECTION_STATE();
                 
        __delay_ms(500);
}
void MODEM_SMS_READ(void)
{
        Send_String_UART1("AT+CMGR=");
        Send_One_Char(message_index);
        
        Send_One_Char('\r');
        
  
        timerHandler=5000;
        timerElapsed=0;
        temp=0;
   
        while((ParseBuffer(receivedBuffer,"Informacije")==0)&&(timerElapsed==0));
        
        temp=ParseBuffer(receivedBuffer,"Informacije");
        
        
        if(temp!=0)
        {
            state = SEARCH_PHONEBOOK;
        }
            
        BACK_TO_CHECK_CONNECTION_STATE();
}
void MODEM_SEARCH_PHONEBOOK(void)
{
    Send_String_UART1("AT+CPBF=");
    Send_String_UART1("Tatjana");
    Send_One_Char('\r');
    timerHandler=3000;
    timerElapsed=0;
    temp=0;
    
    while(ParseBuffer(receivedBuffer,"+CPBF:")==0 & (timerElapsed==0));
    temp = ParseBuffer(receivedBuffer, "Tatjana");
 
    if (temp!=0)
    {
        temp_phone=ParseBuffer(receivedBuffer,",\"+");
         state = SMS_SEND;
    }
    //ocisti se string koji cuva broj
    for(number=0; number < 16; number++)
        {
            phone_number[number]=0;
        }
                
    for(number=0; receivedBuffer[temp_phone+number] != ','; number++) //trazi sve do narednog zareza
       {
           phone_number[number] = receivedBuffer[temp_phone+number];
       }
    
    BACK_TO_CHECK_CONNECTION_STATE();
}
void MODEM_SMS_SEND(char *color, char *temperature)
{
        Send_String_UART1("AT+CNMI=1,0,0,0,1\r");
        __delay_ms(100);
            
        ClearBuffer();
        
        Send_String_UART1("AT+CMGS=");
        Send_String_UART1(phone_number);
       
        Send_One_Char('\r');
                
       
        timerHandler=12000;
        timerElapsed=0;
        temp=0;
            
        while((ParseBuffer(receivedBuffer,">")==0)&&(timerElapsed==0));  
        temp=ParseBuffer(receivedBuffer,">");
        
            
        if(temp!=0)
        {
            Send_String_UART1("Current Temperature: ");
            
            Send_String_UART1(temperature);
            //Send_One_Char(248);
            Send_One_Char(' ');
            Send_One_Char('C');           
            Send_One_Char('\n');
            Send_String_UART1("Color: ");
            Send_String_UART1(color);         
                               
            Send_One_Char('\r');
            Send_One_Char(26);
        }
        
            
        timerHandler=10000;       
        timerElapsed=0;
        temp=0;
           
        while((ParseBuffer(receivedBuffer,"OK")==0)&&(timerElapsed==0));
            
        temp=(ParseBuffer(receivedBuffer,"OK"));
            
        if(temp!=0)
        {
            state=SMS_DELETE;
        }
            
        BACK_TO_CHECK_CONNECTION_STATE();
}
void MODEM_SMS_DELETE(void)
{
    Send_String_UART1("AT+CMGD=");
    Send_One_Char(message_index);
    Send_One_Char('\r');
                  
    timerHandler=2000;
    timerElapsed=0;
    temp=0;
   
    while((ParseBuffer(receivedBuffer,"OK")==0)&&(timerElapsed==0));
                
    temp=ParseBuffer(receivedBuffer,"OK");
                
    if(temp!=0)
    {
        state=CONNECTION_CHECK;
    }
                
    BACK_TO_CHECK_CONNECTION_STATE();
}


/**
  Section: UART Driver Client Routines
*/






