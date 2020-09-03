/*
 * ADS1115.c
 *
 *  Created on: 3 de set de 2020
 *      Author: leobesen
 */

#include "ADS1115.h"
#include "i2c.h"

uint8_t ADSwrite[3];

void ADS1115_config(void){

	// Write to Config register:
	ADSwrite[0] = CONFIG_REGISTER;
	ADSwrite[1] = 0xC1; // MSB of the Config register to be written 11000001
	ADSwrite[2] = 0x83; // LSB of the Config register to be written 10000011
	HAL_I2C_Master_Transmit(&hi2c3, 0x48<<1,ADSwrite,3,1000);

	HAL_Delay(100);

	// Write to Address Pointer register:
	ADSwrite[0] = CONVERSION_REGISTER;
	HAL_I2C_Master_Transmit(&hi2c3, 0x48<<1,ADSwrite,1,1000);
	HAL_Delay(20);
	ADSwrite[0] = 0;
	ADSwrite[1] = 0;
	HAL_I2C_Master_Receive(&hi2c3, 0x48<<1,ADSwrite,2,10000);

//	ADSwrite[0] = CONFIG_REGISTER;
//	ADSwrite[1] = 0;
//	ADSwrite[2] = 0;
//	HAL_I2C_Master_Transmit(&hi2c3, (0x48<<1),&ADSwrite[0],1,100);
//	HAL_Delay(1);
//	HAL_I2C_Master_Receive(&hi2c3, 0x48<<1,&ADSwrite[1],2,1000);

	__NOP();

}
void ADS1115_read(uint16_t* result){


	// Write to Config register:
	ADSwrite[0] = CONFIG_REGISTER;
	HAL_I2C_Master_Transmit(&hi2c3, ADS1115_ADDRESS_READ,ADSwrite,1,100);
	ADSwrite[0] = CONFIG_REGISTER;

	ADSwrite[1] = 0x41; // MSB of the Config register to be written 11000001
	ADSwrite[2] = 0x23; // LSB of the Config register to be written 10000011
	HAL_I2C_Master_Transmit(&hi2c3, ADS1115_ADDRESS_WRITE,ADSwrite,3,100);


	// Write to Address Pointer register:
	ADSwrite[0] = CONVERSION_REGISTER;
	HAL_I2C_Master_Transmit(&hi2c3, ADS1115_ADDRESS_WRITE,ADSwrite,1,100);
	HAL_Delay(1);
	HAL_I2C_Master_Receive(&hi2c3, ADS1115_ADDRESS_READ,ADSwrite,2,100);

}



