/*
 * MAX30102.c
 *
 *  Created on: 24 de jan de 2021
 *      Author: leobesen
 */

#include "MAX30102.h"
#include <stdlib.h>     /* abs */

float temperature;
//uint16_t IR, RED;

void begin(void){

	  uint8_t temp = 0;

	  /*
	   * Reset
	   * */
	  read_byte( MAX30102_MODE_CONFIG, &temp );

	  temp |= (1<<6);
	  write_byte(MAX30102_MODE_CONFIG, temp);

	  //Wait for the end of reset state/process
	  do
	  {
		read_byte(MAX30102_MODE_CONFIG, &temp);
	    temp &= (1<<6);
	  }
	  while (temp != 0);

	  /*
	   * FIFO Config
	   * */
	  write_byte(MAX30102_FIFO_CONFIG, 0x0F);  //sample avg = 1, fifo rollover=false, fifo almost full = 17
	  write_byte(MAX30102_FIFO_WR_PTR, 0x00);
	  write_byte(MAX30102_OVRFLOW_CTR, 0x00);
	  write_byte(MAX30102_FIFO_RD_PTR, 0x00);

	  /*
	   * Mode Configuration
	   * */
	  write_byte(MAX30102_MODE_CONFIG, 0x03); //0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
	 // read_byte( MAX30102_MODE_CONFIG, &temp ); //check

	  /*
	   * SPO2 high resolution activation
	   * */
	  write_byte(MAX30102_SPO2_CONFIG, 0x05); // SPO2_ADC range = 2048nA, SPO2 sample rate (100 Hz), LED pulseWidth 118us
	  //read_byte( MAX30102_SPO2_CONFIG, &temp );

	  /*
	   * LED RED current level
	   * */
	  write_byte(MAX30102_LED1_PA, 0x1F); //Choose value for ~ 6.2mA for LED1
	  //read_byte( MAX30102_LED1_PA, &temp ); //check

	  /*
	   * LED IR current level
	   * */
	  write_byte(MAX30102_LED2_PA, 0x1F); // Choose value for ~ 6.2mA for LED2
	 // read_byte( MAX30102_LED2_PA, &temp ); //check

}


void readFIFO(uint16_t* RED, uint16_t* IR){
   /* Burst read four bytes from the FIFO
    * to obtain 16-bit ADC values for IR
    * and RED reflectance.
    */
	uint8_t data[6];
	uint32_t ir_data = 0;
	uint32_t red_data = 0;
	uint32_t un_temp;

	data[0] = MAX30102_FIFO_DATA;
	HAL_I2C_Master_Transmit(&hi2c3, MAX30102_ADDR, data, 1, 100);
	HAL_I2C_Master_Receive(&hi2c3, MAX30102_ADDR, data, 6, 1000); // Read four times from the FIFO

	un_temp=(unsigned char) data[0];
	un_temp<<=16;
	red_data+=un_temp;
	un_temp=(unsigned char) data[1];
	un_temp<<=8;
	red_data+=un_temp;
	un_temp=(unsigned char) data[2];
	red_data+=un_temp;

	un_temp=(unsigned char) data[3];
	un_temp<<=16;
	ir_data+=un_temp;
	un_temp=(unsigned char) data[4];
	un_temp<<=8;
	ir_data+=un_temp;
	un_temp=(unsigned char) data[5];
	ir_data+=un_temp;

	red_data&=0x03FFFF;  //Mask MSB [23:18]
	ir_data&=0x03FFFF;  //Mask MSB [23:18]

	red_data >>= 2; // the result is left aligned, starting on bit 18. Thus, by using 16 bits ADC resolution, we need to shift right by 2.
	ir_data >>= 2;

	// Combine bytes to get the actual number
	IR[0] = (uint16_t)(ir_data);
	RED[0] = (uint16_t)(red_data);
}

uint8_t read_diodes(uint16_t* ir_buff, uint16_t* red_buff)
{
    uint8_t i, sampleNum = 0;
    uint8_t wrPtr[1] = {0}, rdPtr[1] = {0}, ovPtr, _ptr, samples[4] = {0};
    uint8_t data[4]  = {0};

    read_byte( MAX30102_FIFO_WR_PTR, &wrPtr[0] );
    read_byte( MAX30102_FIFO_RD_PTR, &rdPtr[0] );

    sampleNum = abs( 16 + wrPtr[0] - rdPtr[0] ) % 16;

    if ( sampleNum >= 1 )
    {
        for ( i = 0; i < sampleNum; ++i )
        {
            // read data
            data[0] = MAX30102_FIFO_DATA;
            HAL_I2C_Master_Transmit(&hi2c3, MAX30102_ADDR, data, 1, 100);
            HAL_I2C_Master_Receive(&hi2c3, MAX30102_ADDR, samples, 4, 100); // Read four times from the FIFO

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
    read_byte(MAX30102_INT_STATUS_1, &read_data );
    return read_data;
}



void readTemperature(){

   uint8_t tempInt, tempFrac, reg, temp;

   read_byte( MAX30102_MODE_CONFIG, &temp );
   temp |= (1<<3);
   write_byte(MAX30102_MODE_CONFIG, temp);

   // read temperature integer
   reg = MAX30102_TEMP_INTG;
   HAL_I2C_Master_Transmit(&hi2c3, MAX30102_ADDR, &reg, 1, 100);
   HAL_I2C_Master_Receive(&hi2c3, MAX30102_ADDR, &tempInt, 1, 100);


   // read temperature fraction
   reg = MAX30102_TEMP_FRAC;
   HAL_I2C_Master_Transmit(&hi2c3, MAX30102_ADDR, &reg, 1, 100);
   HAL_I2C_Master_Receive(&hi2c3, MAX30102_ADDR, &tempFrac, 1, 100);

   temperature  = (float) tempFrac * 0.0625 + (int8_t) tempInt;
}

void write_byte(uint8_t reg, uint8_t data){
   /*  Write to the register of a device. Note the char
    *  type must be used due to the MBED i2c functions
    */
   uint8_t transfer[2] = {reg, data};
   HAL_I2C_Master_Transmit(&hi2c3, MAX30102_ADDR, transfer, 2, 100);
}

void read_byte(uint8_t reg, uint8_t* data){

	   HAL_I2C_Master_Transmit(&hi2c3, MAX30102_ADDR + 1 , &reg, 1, 100);
	   HAL_I2C_Master_Receive(&hi2c3, MAX30102_ADDR + 1, data, 1, 100);
}




