/*
 * MAX30102.h
 *
 *  Created on: 24 de jan de 2021
 *      Author: leobesen
 */

#ifndef INC_MAX30102_H_
#define INC_MAX30102_H_

#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "i2c.h"

// 8bit I2C address of the MAX30100
// with R/W bit set
#define MAX30102_ADDR           0xAE

// Registers
#define MAX30102_INT_STATUS_1     		0x00  // Which interrupts are tripped - PPG
#define MAX30102_INT_STATUS_2     		0x01  // Which interrupts are tripped - TEMP
#define MAX30102_INT_ENABLE_1     		0x02  // Which interrupts are active - PPG
#define MAX30102_INT_ENABLE_2     		0x03  // Which interrupts are active - TEMP
#define MAX30102_FIFO_WR_PTR      		0x04  // Where data is being written
#define MAX30102_OVRFLOW_CTR      		0x05  // Number of lost samples
#define MAX30102_FIFO_RD_PTR      		0x06  // Where to read from
#define MAX30102_FIFO_DATA        		0x07  // Ouput data buffer
#define MAX30102_FIFO_CONFIG			0x08
#define MAX30102_MODE_CONFIG      		0x09  // Control register
#define MAX30102_SPO2_CONFIG      		0x0A  // Oximetry settings
#define MAX30102_LED1_PA	      		0x0C  // Pulse width and power of LEDs
#define MAX30102_LED2_PA	      		0x0D  // Pulse width and power of LEDs
#define MAX30102_MULTI_LED_CTRL_SLOT12	0x11  // Pulse width and power of LEDs
#define MAX30102_MULTI_LED_CTRL_SLOT34	0x12  // Pulse width and power of LEDs
#define MAX30102_TEMP_INTG      		0x1F  // Temperature value, whole number
#define MAX30102_TEMP_FRAC      		0x20  // Temperature value, fraction
#define MAX30102_TEMP_CONFIG     		0x21  // Temperature value, fraction
#define MAX30102_REV_ID        	 		0xFE  // Part revision
#define MAX30102_PART_ID        		0xFF  // Part ID, normally 0x15


typedef enum {
   HR_mode = 0x02, // IR only
   SPO2_mode       // RED and IR
} modeControl_t;

typedef enum{ // This is the same for both LEDs
   pw200,    // 200us pulse
   pw400,    // 400us pulse
   pw800,    // 800us pulse
   pw1600    // 1600us pulse
} pulseWitdth_t;

typedef enum{
   sr50,    // 50 samples per second
   sr100,   // 100 samples per second
   sr167,   // 167 samples per second
   sr200,   // 200 samples per second
   sr400,   // 400 samples per second
   sr600,   // 600 samples per second
   sr800,   // 800 samples per second
   sr1000   // 1000 samples per second
} sampleRate_t;

typedef enum{
   i0,    // No current
   i4,    // 4.4mA
   i8,    // 7.6mA
   i11,   // 11.0mA
   i14,   // 14.2mA
   i17,   // 17.4mA
   i21,   // 20.8mA
   i24,   // 24 mA
   i27,   // 27.1mA
   i31,   // 30.6mA
   i34,   // 33.8mA
   i37,   // 37.0mA
   i40,   // 40.2mA
   i44,   // 43.6mA
   i47,   // 46.8mA
   i50    // 50.0mA
} ledCurrent_t;

float getTemperatureC();
uint16_t getIR();
uint16_t getRED();
void begin(void);
void startTemperatureSampling();
void readFIFO(uint16_t* RED, uint16_t* IR);
void readTemperature();
void write_byte(uint8_t reg, uint8_t data);
void read_byte(uint8_t reg, uint8_t* data);

uint8_t read_diodes(uint16_t* ir_buff, uint16_t* red_buff);
uint8_t get_status();


#endif /* INC_MAX30102_H_ */
