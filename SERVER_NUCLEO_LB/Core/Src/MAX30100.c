/*
 * MAX30100.c
 *
 *  Created on: Jan 30, 2020
 *      Author: leobesen
 */
#include "MAX30100.h"
#include <stdlib.h>     /* abs */

float temperature;
//uint16_t IR, RED;

void begin(void){

	  uint8_t temp = 0;
	  uint8_t mode = 0x03; // [0,7]
	  bool temperature = true;
	  bool spo2_hs = true;
	  uint8_t spo2_sr = 6; // [0,7]
	  uint8_t pulse_width = 3; // [0,3]
	  uint8_t red_level = 0x05; // 0b00001111; // [0,15]
	  uint8_t ir_level = 0x05; // [0,15]
	  uint8_t hr_ready_int = true;
	  uint8_t spo2_ready_int = true;
	  uint8_t temp_ready_int = true;


	  //Reset
	  read_byte( MAX30100_MODE_CONFIG, &temp );

	  temp |= (1<<6);
	  write_byte(MAX30100_MODE_CONFIG, temp);

	  //Wait for the end of reset state/process
	  do
	  {
		read_byte(MAX30100_MODE_CONFIG, &temp);
	    temp &= (1<<6);
	  }
	  while (temp != 0);

	  //Mode Configuration
	  read_byte( MAX30100_MODE_CONFIG, &temp );

	  temp |= mode;
	  write_byte(MAX30100_MODE_CONFIG, temp);
	  temp = 0;
	  read_byte( MAX30100_MODE_CONFIG, &temp ); //check

	  //Temperature sensor activation
	  read_byte( MAX30100_MODE_CONFIG, &temp );

	  if ( temperature == true )
	    temp |= (1<<3);
	  else
	    temp &= ~(1<<3);

	  write_byte(MAX30100_MODE_CONFIG, temp);
	  temp = 0;
	  read_byte( MAX30100_MODE_CONFIG, &temp ); //check

	  //SPO2 high resolution activation
	  read_byte( MAX30100_SPO2_CONFIG, &temp );

	  if (spo2_hs == true)
	    temp |= (1<<6);

	  else
	    temp &= ~(1<<6);
	  write_byte(MAX30100_SPO2_CONFIG, temp);

	  //SPO2 sample rate configuration
	  read_byte( MAX30100_SPO2_CONFIG, &temp );
	  temp |= (spo2_sr << 2);

	  write_byte( MAX30100_SPO2_CONFIG, temp );
	  temp = 0;
	  read_byte( MAX30100_SPO2_CONFIG, &temp ); //check

	  //LED pulse width configuration
	  read_byte( MAX30100_SPO2_CONFIG, &temp );

	  temp |= pulse_width;
	  write_byte(MAX30100_SPO2_CONFIG, temp);
	  temp = 0;
	  read_byte( MAX30100_SPO2_CONFIG, &temp ); //check

	  //LED red current level
	  read_byte( MAX30100_LED_CONFIG, &temp );

	  temp |= (red_level << 4);
	  write_byte(MAX30100_LED_CONFIG, temp);
	  temp = 0;
	  read_byte( MAX30100_LED_CONFIG, &temp ); //check

	  //LED ir current level
	  read_byte( MAX30100_LED_CONFIG, &temp );

	  temp |= ir_level;
	  write_byte(MAX30100_LED_CONFIG, temp);
	  temp = 0;
	  read_byte( MAX30100_LED_CONFIG, &temp ); //check

	  //Heart rate data ready interruption
	  read_byte( MAX30100_INT_ENABLE, &temp );
	  if (hr_ready_int == true)
	    temp |= (1<<5);
	  else
	    temp &= ~(1<<5);

	  write_byte(MAX30100_INT_ENABLE, temp);
	  temp = 0;
	  read_byte( MAX30100_INT_ENABLE, &temp ); //check

	  //SPO2 data ready interruption
	  read_byte( MAX30100_INT_ENABLE, &temp );
	  if (spo2_ready_int == true)
	    temp |= (1<<4);
	  else
	    temp &= ~(1<<4);

	  write_byte(MAX30100_INT_ENABLE, temp);
	  temp = 0;
	  read_byte( MAX30100_INT_ENABLE, &temp ); //check

	  //Temperature data ready interruption
	  read_byte(MAX30100_INT_ENABLE, &temp );
	  if (temp_ready_int == true)
	    temp |= (1<<6);
	  else
	    temp &= ~(1<<6);

	  write_byte(MAX30100_INT_ENABLE, temp);
	  temp = 0;
	  read_byte( MAX30100_INT_ENABLE, &temp ); //check
}


void readFIFO(uint16_t* RED, uint16_t* IR){
   /* Burst read four bytes from the FIFO
    * to obtain 16-bit ADC values for IR
    * and RED reflectance.
    */
	uint8_t data[4];
   data[0] = MAX30100_FIFO_DATA;
   HAL_I2C_Master_Transmit(&hi2c3, MAX30100_ADDR, data, 1, 100);
   HAL_I2C_Master_Receive(&hi2c3, MAX30100_ADDR, data, 4, 1000); // Read four times from the FIFO

   // Combine bytes to get the actual number
   IR[0] = (data[0]<<8) | data[1];
   RED[0] = (data[2]<<8) | data[3];
}

uint8_t read_diodes(uint16_t* ir_buff, uint16_t* red_buff)
{
    uint8_t i, sampleNum = 0;
    uint8_t wrPtr[1] = {0}, rdPtr[1] = {0}, ovPtr, _ptr, samples[4] = {0};
    uint8_t data[4]  = {0};

    read_byte( MAX30100_FIFO_WR_PTR, &wrPtr[0] );
    read_byte( MAX30100_FIFO_RD_PTR, &rdPtr[0] );

    sampleNum = abs( 16 + wrPtr[0] - rdPtr[0] ) % 16;

    if ( sampleNum >= 1 )
    {
        for ( i = 0; i < sampleNum; ++i )
        {
            // read data
            data[0] = MAX30100_FIFO_DATA;
            HAL_I2C_Master_Transmit(&hi2c3, MAX30100_ADDR, data, 1, 100);
            HAL_I2C_Master_Receive(&hi2c3, MAX30100_ADDR, samples, 4, 100); // Read four times from the FIFO

            *(ir_buff) =  (uint16_t)samples[1];
            *(ir_buff++) |= (uint16_t)samples[0] << 8;
            *(red_buff) = (uint16_t)samples[3];
            *(red_buff++) |=  (uint16_t) samples[2] << 8;

        }
    }
    return sampleNum;
}

uint8_t get_status()
{
    uint8_t read_data;
    read_byte(MAX30100_INT_STATUS, &read_data );
    return read_data;
}



void readTemperature(){

   uint8_t tempInt, tempFrac, reg, temp;

   read_byte( MAX30100_MODE_CONFIG, &temp );
   temp |= (1<<3);
   write_byte(MAX30100_MODE_CONFIG, temp);

   // read temperature integer
   reg = MAX30100_TEMP_INTG;
   HAL_I2C_Master_Transmit(&hi2c3, MAX30100_ADDR, &reg, 1, 100);
   HAL_I2C_Master_Receive(&hi2c3, MAX30100_ADDR, &tempInt, 1, 100);


   // read temperature fraction
   reg = MAX30100_TEMP_FRAC;
   HAL_I2C_Master_Transmit(&hi2c3, MAX30100_ADDR, &reg, 1, 100);
   HAL_I2C_Master_Receive(&hi2c3, MAX30100_ADDR, &tempFrac, 1, 100);

   temperature  = (float) tempFrac * 0.0625 + (int8_t) tempInt;
}

void write_byte(uint8_t reg, uint8_t data){
   /*  Write to the register of a device. Note the char
    *  type must be used due to the MBED i2c functions
    */
   uint8_t transfer[2] = {reg, data};
   HAL_I2C_Master_Transmit(&hi2c3, MAX30100_ADDR, transfer, 2, 100);
}

void read_byte(uint8_t reg, uint8_t* data){

	   HAL_I2C_Master_Transmit(&hi2c3, MAX30100_ADDR, &reg, 1, 100);
	   HAL_I2C_Master_Receive(&hi2c3, MAX30100_ADDR, data, 1, 100);
}


