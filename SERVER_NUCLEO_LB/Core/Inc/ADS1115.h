/*
 * ADS1115.h
 *
 *  Created on: 3 de set de 2020
 *      Author: leobesen
 */

#ifndef INC_ADS1115_H_
#define INC_ADS1115_H_

#include "main.h"


/*
 * ADS1115 Config
 * */
#define ADS1115_ADDRESS_WRITE (0x48 << 1) | 0
#define ADS1115_ADDRESS_READ (0x48 << 1) | 1

#define CONVERSION_REGISTER 0X00 // 16-bit conversion result
#define CONFIG_REGISTER 0x01
/*
 * { OS , MUX[2:0] , PGA[2:0] , MODE , DR[2:0] , COMP_MODE , COMP_POL , COMP_LAT , COMP_QUE[1:0] }
 * OS -> write 1 = start single conversion; read = status of the conversion
 * MUX -> These bits configure the input multiplexer. 100 : AINP = AIN0 and AINN = GND
 * PGA -> programmable gain amplifier - FSR (full scale range). 000 : FSR = ±6.144V
 * MODE -> 0 : Continuous-conversion mode, 1 : Single-shot mode or power-down state (default)
 * DR -> Data rate. 001 : 16 SPS
 * COMP_MODE -> Comparator mode
 * COMP_POL -> Comparator polarity
 * COMP_LAT -> Latching comparator
 * COMP_QUE -> Comparator queue and disable
 * */
/*
 * 1 100 000 1 001 0 0 0 11 -> 1100000100100011 -> MSB: 0xC1; LSB: 0x23
 * */
#define LO_THRESH 0x02 // lower threshold values used by the comparator
#define HI_THRESH 0x03 // upper threshold values used by the comparator

/* *
 * Application
 * */

/*
1.Write to Config register:
	–  First byte: 0b10010000 (first 7-bit I2C address followed by a low R/W bit)
	–  Second byte: 0b00000001 (points to Config register)
	–  Third byte: 0b10000100 (MSB of the Config register to be written)
	–  Fourth byte: 0b10000011 (LSB of the Config register to be written)

2.Write to Address Pointer register:
	–  First byte: 0b10010000 (first 7-bit I2C address followed by a low R/W bit)
	–  Second byte: 0b00000000 (points to Conversion register)

3.Read Conversion register:
	–  First byte: 0b10010001 (first 7-bit I2C address followed by a high R/W bit)
	–  Second byte: the ADS111x response with the MSB of the Conversion register
	–  Third byte: the ADS111x response with the LSB of the Conversion register
*/

void ADS1115_config(void);
void ADS1115_read(uint16_t*);



#endif /* INC_ADS1115_H_ */
