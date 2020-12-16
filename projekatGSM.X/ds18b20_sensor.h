/* 
 * File:   ds18b20_sensor.h
 * Author: Tatjana
 *
 * Created on November 16, 2020, 1:11 AM
 */

#ifndef DS18B20_SENSOR_H
#define	DS18B20_SENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* DS18B20_SENSOR_H */
#include "mcc_generated_files/clock.h"
//uint16_t raw_temp;
extern char t[10];
//extern char temperature[];
//extern unsigned char temperature[];
void PIN_Init(void);

//used to know if the DS18B20 sensor is correctly connected to the circuit, returns 1 if OK and 0 if error
uint8_t ds18b20_start();




//writes 1 byte (8 bits) to the DS18B20 sensor, this function is based on the previous function. 
//This function writes LSB (Least Significant Bit) first.
void ds18b20_write_byte(uint8_t value);



//reads 1 byte (8 bits) from the DS18B20 sensor, this function is based on the previous function. 
//This function reads LSB first.
uint8_t ds18b20_read_byte(void);

//reads the temperature raw data which is 16-bit long (two 8-bit registers), 
//the data is stored in the variable raw_temp_value, returns 1 if OK and 0 if error.
uint8_t ds18b20_read(uint16_t *raw_temp_value);

void temperature_uart (char *temperature);

