/*
 * LIS3DH_I2C.c
 *
 *  Created on: 21 de mai de 2020
 *      Author: leobesen
 */

#include "LIS3DH_I2C.h"


#if OLD_REV
    float   fs_factor;  // full scale factor
#else
    uint8_t fs_factor;  // full scale factor
#endif
    uint8_t    dt[2];      // working buffer
    uint8_t acc_addr;   // acc sensor address
    uint8_t acc_id;     // acc ID
    uint8_t acc_ready;  // acc is on I2C line = 1, not = 0

void begin_LIS3DH_I2C(uint8_t addr, uint8_t data_rate, uint8_t fullscale){
	initialize (addr, data_rate, fullscale);
}

void initialize (uint8_t addr, uint8_t data_rate, uint8_t fullscale){
	// Check acc is available of not
	acc_addr = addr;
	dt[0] = LIS3DH_WHO_AM_I;

	HAL_I2C_Master_Transmit(&hi2c3, acc_addr | 0x01, dt, 1, 1000);
	HAL_I2C_Master_Receive(&hi2c3, acc_addr | 0x01, dt, 1, 1000);

	if (dt[0] == I_AM_LIS3DH) {
		acc_ready = 1;
	} else {
		acc_ready = 0;
		return;     // acc chip is NOT on I2C line then terminate
	}
	//  Reg.1
	dt[0] = LIS3DH_CTRL_REG1;
	dt[1] = 0x07;   //0000 0111
	dt[1] |= data_rate << 4;    // 0000 0111 |= 0010 0000  ---> 0010 0111;  10Hz, high res, xyz enbabled.

	//note:  dt[1]=0x0F;    //low resolution; @ datarate;  Must also zero out CTRL_REG4[3]
	//dt[1] = 0x07;
	HAL_I2C_Master_Transmit(&hi2c3, acc_addr, dt, 2, 100);


	//  Reg.4
	dt[0] = LIS3DH_CTRL_REG4;
	dt[1] = 0x08;  // High resolution       //0000 1000
	dt[1] |= fullscale << 4;                //
	//fullscale = 0000 0010
	//fullscale << 4 = 0010 0000
	//result of OR = 0010 1000   ---->
	//  continuous update
	//  endian, LSB @ lowest address
	//  10 = 8G scale
	//  High resolution (10bit) ENABLED
	//  0, 0 N/A

	HAL_I2C_Master_Transmit(&hi2c3, acc_addr, dt, 2, 100);

	switch (fullscale) {
		case LIS3DH_FS_2G:
			fs_factor = LIS3DH_SENSITIVITY_2G;
			break;
		case LIS3DH_FS_4G:
			fs_factor = LIS3DH_SENSITIVITY_4G;
			break;
		case LIS3DH_FS_8G:
			fs_factor = LIS3DH_SENSITIVITY_8G;
			break;
		case LIS3DH_FS_16G:
			fs_factor = LIS3DH_SENSITIVITY_16G;
			break;
		default:
			break;
	}
}

void read_reg_data(uint8_t *data){
        // X,Y & Z
        // manual said that
        // In order to read multiple bytes, it is necessary to assert the most significant bit
        // of the subaddress field.
        // In other words, SUB(7) must be equal to ‘1’ while SUB(6-0) represents the address
        // of the first register to be read.
        dt[0] = LIS3DH_OUT_X_L | 0x80;
        HAL_I2C_Master_Transmit(&hi2c3, acc_addr, dt, 1, 100);
        HAL_I2C_Master_Receive(&hi2c3, acc_addr, data, 6, 100);
}

void read_mg_data(float *dt_usr){

	uint8_t data[6];

	if (acc_ready == 0) {
		dt_usr[0] = 0;
		dt_usr[1] = 0;
		dt_usr[2] = 0;
		return;
	}
	read_reg_data(data);
	// change data type
	#if OLD_REV // Fixed bugs -> (1) unit is not mg but g (2) shift right 4bit = /16
		dt_usr[0] = (float)((short)((data[1] << 8) | data[0])) * fs_factor / 15;
		dt_usr[1] = (float)((short)((data[3] << 8) | data[2])) * fs_factor / 15;
		dt_usr[2] = (float)((short)((data[5] << 8) | data[4])) * fs_factor / 15;
	#else
		dt_usr[0] = (float)((short)((data[1] << 8) | data[0]) >> 4) * fs_factor;
		dt_usr[1] = (float)((short)((data[3] << 8) | data[2]) >> 4) * fs_factor;
		dt_usr[2] = (float)((short)((data[5] << 8) | data[4]) >> 4) * fs_factor;
	#endif
}

void read_data(float *dt_usr){
	uint8_t data[6];

	if (acc_ready == 0) {
		dt_usr[0] = 0;
		dt_usr[1] = 0;
		dt_usr[2] = 0;
		return;
	}
	read_reg_data(data);
	// change data type
	#if OLD_REV // Fixed bugs -> shift right 4bit = /16 (not /15)
		dt_usr[0] = (float)((short)((data[1] << 8) | data[0])) * fs_factor / 15 * GRAVITY;
		dt_usr[1] = (float)((short)((data[3] << 8) | data[2])) * fs_factor / 15 * GRAVITY;
		dt_usr[2] = (float)((short)((data[5] << 8) | data[4])) * fs_factor / 15 * GRAVITY;
	#else
		dt_usr[0] = (float)((short)((data[1] << 8) | data[0]) >> 4) * fs_factor * GRAVITY;
		dt_usr[1] = (float)((short)((data[3] << 8) | data[2]) >> 4) * fs_factor * GRAVITY;
		dt_usr[2] = (float)((short)((data[5] << 8) | data[4]) >> 4) * fs_factor * GRAVITY;
	#endif
}

uint8_t read_id(){
	dt[0] = LIS3DH_WHO_AM_I;
	HAL_I2C_Master_Transmit(&hi2c3, acc_addr, dt, 1, 100);
	HAL_I2C_Master_Receive(&hi2c3, acc_addr, dt, 1, 100);
	return (uint8_t)dt[0];
}

uint8_t  data_ready(){
	if (acc_ready == 1) {
		dt[0] = LIS3DH_STATUS_REG_AUX;
		HAL_I2C_Master_Transmit(&hi2c3, acc_addr, dt, 1, 100);
		HAL_I2C_Master_Receive(&hi2c3, acc_addr, dt, 1, 100);
		if (!(dt[0] & 0x01)) {
			return 0;
		}
	}
	return 1;
}


uint8_t  read_reg(uint8_t addr){
	if (acc_ready == 1) {
		dt[0] = addr;
		HAL_I2C_Master_Transmit(&hi2c3, acc_addr, dt, 1, 100);
		HAL_I2C_Master_Receive(&hi2c3, acc_addr, dt, 1, 100);
	} else {
		dt[0] = 0xff;
	}
	return (uint8_t)dt[0];
}

void write_reg(uint8_t addr, uint8_t data){
	if (acc_ready == 1) {
		dt[0] = addr;
		dt[1] = data;
		HAL_I2C_Master_Transmit(&hi2c3, acc_addr, dt, 2, 100);
	}
}

