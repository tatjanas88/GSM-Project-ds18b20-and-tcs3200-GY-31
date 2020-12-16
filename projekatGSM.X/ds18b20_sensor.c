#include "ds18b20_sensor.h"
#include <string.h>

uint16_t raw_temp;

//default 16 bit resolution
char t[10];


void PIN_Init(void)
{
    TRISEbits.TRISE6 = 0;   //data temp sens
    LATEbits.LATE6 = 1;
    
    //TRISCbits.TRISC13=0;     //led check
    //LATCbits.LATC13=0;
    //PORTCbits.RC13 = 0;
}

uint8_t ds18b20_start(void)
{
    TRISEbits.TRISE6 = 0;   //kontroler povuce magistralu na 0 480us
    LATEbits.LATE6 = 0;
    __delay_us(480);
    
    
    TRISEbits.TRISE6 = 1;   //pin ulazni
    __delay_us(60);         //ceka 80us da sens odgovori
    
    
    if(PORTEbits.RE6==0)
    {
        __delay_us(480);
        return 0;    
    }    
    else
    {
        __delay_us(480);
        return 1;
    }
}


void ds18b20_write_byte(uint8_t data)
{
    TRISEbits.TRISE6=0; //output
    int i=0;
    
    for(i=0; i<8; i++)
    {
        if((data & (1<<i)) !=0)         //write 1
        {
            LATEbits.LATE6=0;
            __delay_us(6);
            LATEbits.LATE6=1;
            __delay_us(64);
        }
        
        else                            //write 0
        {
            LATEbits.LATE6=0;
            __delay_us(60);
            LATEbits.LATE6=1;
            __delay_us(10);
        }            
    }
}

uint8_t ds18b20_read_byte(void)
{
    char value=0;
    TRISEbits.TRISE6=1; //input
    int i=0;
    
    for(i=0; i<8; i++)
    {
        TRISEbits.TRISE6=0;
        LATEbits.LATE6=0;
        __delay_us(6);//2  
        TRISEbits.TRISE6=1;             
         __delay_us(9);
        
        if(PORTEbits.RE6!=0)
        {
           
            value |= 1<<i;              //read 1 here
        }
        __delay_us(55);
    }
    
    return value;
}

uint8_t ds18b20_read(uint16_t *raw_temp_value)
{
    if(ds18b20_start()) // send start pulse
    {
        return 0; // return 0 if error
    }
    
    ds18b20_write_byte(0xCC); // send skip ROM command
    ds18b20_write_byte(0x44); // send start conversion command
    
    while(ds18b20_read_byte()==0); // wait for conversion complete
    
    if(ds18b20_start()) // send start pulse
    {
        return 0; // return 0 if error
    }
    
    ds18b20_write_byte(0xCC); // send skip ROM command
    ds18b20_write_byte(0xBE); // send read command
    
    // read temperature LSB byte and store it on raw_temp_value LSB byte
    *raw_temp_value = ds18b20_read_byte();
    
    // read temperature MSB byte and store it on raw_temp_value MSB byte
    *raw_temp_value |=(uint16_t)(ds18b20_read_byte()<<8);
    
    return 1; // OK --> return 1
           
}

 void temperature_uart(char *temperature)
{    
    //temperature[8] = 248; // put degree symbol ( ° ) ascii
    
    if(ds18b20_read(&raw_temp))
    {
        if(raw_temp & 0x8000) // if the temperature is negative
        {
            temperature[0] = '-';
            raw_temp = (~raw_temp) + 1;  // change temperature value to positive form
        }
        else
        {
            
            if((raw_temp >> 4) >= 100)  // if the temperature >= 100 °C
            {
                temperature[0] = '1'; // put 1 of hundreds
            }
            else  // otherwise
            {
                temperature[0] = ' ';// put space ' '
            }
        }
        // put the first two digits ( for tens and ones)
        temperature[1] = ( (raw_temp >> 4) / 10 ) % 10 + '0';  // put tens digit
        temperature[2] =   (raw_temp >> 4)        % 10 + '0';  // put ones digit
        
         // put the 4 fraction digits (digits after the point)
         // why 625?  because we're working with 12-bit resolution (default resolution)
         //0x0f is a hexadecimal representation of a byte. Specifically, the bit pattern 00001111
         // 0x0F --- It represents the lower four bits and translates the the bit-pattern 0000 1111. 
         //& is a bitwise AND operation.
        temperature[4] = ( (raw_temp & 0x0F) * 625) / 1000 + '0';  // put thousands digit
        temperature[5] = (((raw_temp & 0x0F) * 625) / 100 ) % 10 + '0';    // put hundreds digit
        temperature[6] = (((raw_temp & 0x0F) * 625) / 10 )  % 10 + '0';    // put tens digit
        temperature[7] = ( (raw_temp & 0x0F) * 625) % 10 + '0'; // put ones digit
        
         __delay_us(200);
        //return temperature;
    }
    
}

       

