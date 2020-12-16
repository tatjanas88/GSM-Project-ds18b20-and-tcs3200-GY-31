

/**
  Section: Included Files
*/
#include "xc.h"
//#include "p24FJ256GA106.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/clock.h"
#include "ds18b20_sensor.h"
#include "color.h"

#define MAX_STRING_LEN 50
char buff[2][MAX_STRING_LEN];


//-----------------------------------------------
int tempRX;
int i=0,get_values=0;
int count=0;

//-----------------------------------------------

#pragma CONFIG FNOSC = FRCPLL
#pragma CONFIG FWDTEN = OFF 

void __attribute__((interrupt,no_auto_psv)) _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;         
    tempRX = U1RXREG;
    
    for (i=0;i<RXbuffLen1-1;i++)
    {
        receivedBuffer[i]=receivedBuffer[i+1];
    }
    receivedBuffer[RXbuffLen1-1]=tempRX; //popunjava se buffer preko interapta, 
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _T1Interrupt (  )
{
    // Check if the Timer Interrupt/Status is set 
    TMR1 =0;
    IFS0bits.T1IF = 0; //flag status check

    count++;
    if(timerHandler) 
    { 
           timerHandler--; 
    }
    if(timerHandler==0) 
    { 
           timerElapsed=1; 
    }
     if(count==5000)
    {
        get_values=1;
        count=0;
    }

}
 
/*
                         Main application
 */
int main(void)
{
    //take care that MAX_STRING_LEN is big enough, otherways color will overwrite temper.
    char color[MAX_STRING_LEN];
    char temperature[] = "000.0000";
    unsigned int valueR;
    unsigned int valueB;
    unsigned int valueG;
    unsigned int sum = 0;
   
 
    // initialize the device
    SYSTEM_Initialize();
    PIN_Init();
    TCS3200_Config();
    __delay_ms(1000);
    TMR2_init();
    Set_Scaling(Scl100); //postavi se na 2%
    __delay_ms(1000);
    
    ClearBuffer();
   
    
    state = CONNECTION_CHECK;

    while (1)
    {
        //smjestanje ocitane temperature u t[] koja se prosljedjuje u uart1.c
        temperature_uart(temperature);        
        
         __delay_ms(300);
         
        valueR = Measure_Red();
        __delay_ms(300);
        //sprintf(value1," %d   ",valueR);
        valueG = Measure_Green();
        __delay_ms(300);
        //sprintf(value2," %d   ",valueG);
        valueB = Measure_Blue();
        //sprintf(value3," %d   ",valueB);
        __delay_ms(300);
        
        sum = (valueB + valueG + valueR)/3;
        
         if((sum >= 720) && (sum <= 1100))       
        {
             
             //sprintf(color,"%s %d %d %d", "Green",valueR,valueG,valueB);
             strcpy(color, "GREEN");
        }
        if((sum >= 1300) && (sum <= 1700))       
        {
            
             //sprintf(color,"%s %d %d %d", "Blue",valueR,valueG,valueB);
        }
         if((sum >= 550) && (sum <= 719))       
        {
             strcpy(color, "RED");
             //sprintf(color,"%s %d %d %d", "Red",valueR,valueG,valueB);
        }
        if((sum < 550)|| ((sum > 1100) && (sum < 1300)) || (sum > 1700))
        {
            strcpy(color, "CAN'T DETECT");
            //sprintf(color,"%s %d %d %d", "Can't detect",valueR,valueG,valueB);
        }
          
        
        
         
          
      
          
        switch(state)
        {
            case CONNECTION_CHECK:
                MODEM_CHECK_CONNECTION();
                break;
                
            case SMS_FORMAT_DEFINE:
                MODEM_SMS_FORMAT_DEFINE();
                break;
            case SMS_LIST:
                MODEM_SMS_LIST();
                break;
            case SMS_READ:
                MODEM_SMS_READ();
                break;
            case SEARCH_PHONEBOOK:
                MODEM_SEARCH_PHONEBOOK();
                break;
            case SMS_SEND:
                MODEM_SMS_SEND(color, temperature);
                break;
            case SMS_DELETE:
                MODEM_SMS_DELETE();
                break;
                
            default:
                state=CONNECTION_CHECK;
                       
                
        }
        
        
    }

    return 0;
}
/**
 End of File
*/

