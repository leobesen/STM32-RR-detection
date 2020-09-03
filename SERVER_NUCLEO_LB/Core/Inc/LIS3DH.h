
#ifndef __H_LIS3DH__
#define __H_LIS3DH__

//Header files
#include "stm32wbxx_hal.h"
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "spi.h"

//List of Defines


//Device Registers LIS3DH
#define STATUS_REG_AUX         0x07
#define OUT_ADC1_L             0x08
#define OUT_ADC1_H             0x09
#define OUT_ADC2_L             0x0A
#define OUT_ADC2_H             0x0B
#define OUT_ADC3_L             0x0C
#define OUT_ADC3_H             0x0D
#define INT_COUNTER_REG        0x0E //Reserved (do not modify)
#define WHO_AM_I               0x0F //00110011 - 0x33
#define TEMP_CFG_REG           0x1F //Enable ADC and Temperature
#define OFF_X_ADDR             0x10 //Reserved (do not modify)
#define OFF_Y_ADDR             0x11 //Reserved (do not modify)
#define OFF_Z_ADDR             0x12 //Reserved (do not modify)
#define CTRL_REG0              0x1E //Disconnect SDO/SA0 pull-up
#define CTRL_REG1              0x20 //Enable axis | data rate selection | Low power mode
#define CTRL_REG2              0x21 //High-pass filter mode
#define CTRL_REG3              0x22 //Interrupt INT1 config
#define CTRL_REG4              0x23 //Full-scale selection | High-resolution output mode | Self-test enable
#define CTRL_REG5              0x24
#define CTRL_REG6              0x25
#define REFERENCE              0x26
#define STATUS_REG             0x27
#define OUT_X_L                0x28
#define OUT_X_H                0x29
#define OUT_Y_L                0x2A
#define OUT_Y_H                0x2B
#define OUT_Z_L                0x2C
#define OUT_Z_H                0x2D
#define FIFO_CTRL_REG          0x2E
#define FIFO_SRC_REG           0x2F
#define INT1_CFG               0x30
#define INT1_SRC               0x31
#define INT1_THS               0x32
#define INT1_DURATION          0x33
#define CLICK_CFG              0x38
#define CLICK_SRC              0x39
#define CLICK_THS              0x3A
#define TIME_LIMIT             0x3B
#define TIME_LATENCY           0x3C
#define TIME_WINDOW            0x3D

/*  

	CTRL_REG0 config  

*/
#define SDO_PU_DISC_PULLUP			((uint8_t)0x00) //Pull-up connected to SDO/SA0 pin
#define SDO_PU_DISC_NO_PULLUP		((uint8_t)0x80) //Pull-up disconnected to SDO/SA0 pin
/*  

	CTRL_REG1 config  

*/
//Data rate
#define DATARATE_POWERDOWN           ((uint8_t)0x00)  /*Power Down Mode*/
#define DATARATE_1               	 ((uint8_t)0x10)  /* 1    Hz HR / Normal / Low-power Mode */
#define DATARATE_10                	 ((uint8_t)0x20)  /* 10   Hz HR / Normal / Low-power Mode */
#define DATARATE_25                	 ((uint8_t)0x30)  /* 25   Hz HR / Normal / Low-power Mode */
#define DATARATE_50                  ((uint8_t)0x40)  /* 50   Hz HR / Normal / Low-power Mode */
#define DATARATE_100                 ((uint8_t)0x50)  /* 100  Hz HR / Normal / Low-power Mode */
#define DATARATE_200                 ((uint8_t)0x60)  /* 200  Hz HR / Normal / Low-power Mode */
#define DATARATE_400                 ((uint8_t)0x70)  /* 400  Hz HR / Normal / Low-power Mode */
#define DATARATE_1600                ((uint8_t)0x80)  /* 1600 Hz HR / Normal / Low-power Mode */
#define DATARATE_1344                ((uint8_t)0x90)  /* 1344 Hz HR / Normal  and 5376 Hz Low-power Mode */
//Axis enable
#define X_ENABLE                     ((uint8_t)0x01) //Enable X-Axis
#define Y_ENABLE                     ((uint8_t)0x02) //Enable Y-Axis
#define Z_ENABLE                     ((uint8_t)0x04) //Enable Z-Axis
#define XYZ_ENABLE                   ((uint8_t)0x07) //Enable XYZ-Axis

#define LOW_POWER_MODE				 ((uint8_t)0x08) //Low-power mode enable

/*  

	CTRL_REG2 config  

*/
//High-pass filter mode selection
#define HPM_MODE_0					((uint8_t)0x00) //Normal mode (reset by reading REFERENCE (26h))
#define HPM_MODE_1					((uint8_t)0x40) //Reference signal for filtering
#define HPM_MODE_2					((uint8_t)0x80) //Normal mode
#define HPM_MODE_3					((uint8_t)0xC0) //Autoreset on interrupt event

/*  

	CTRL_REG3 config  

*/
#define I1_CLICK					((uint8_t)0x80) //Click interrupt on INT1
#define I1_IA1						((uint8_t)0x40) //IA1 interrupt on INT1
#define I1_IA2						((uint8_t)0x20) //IA2 interrupt on INT1
#define I1_ZYXDA					((uint8_t)0x10) //ZYXDA interrupt on INT1
#define I1_321DA					((uint8_t)0x08) //321DA interrupt on INT1
#define I1_WTM						((uint8_t)0x04) //FIFO watermark interrupt on INT1
#define I1_OVERRUN					((uint8_t)0x02) //FIFO overrun interrupt on INT1
/*
	In CTRL_REG3 you can configure which feature can generate interrupt on interrupt pin INT1.
	I1_IA1 bit enable interrupt from Interrupt Generator 1 which can be configured by registers INT1_CFG, INT1_THS, INT1_DURATION.
	I1_IA2 bit enable interrupt from Interrupt Generator 2 which can be configured by registers INT2_CFG, INT2_THS, INT2_DURATION.
	I1_ZYXDA bit enable data ready interrupt, it means when new data are enable the interrupt is triggered.
*/

/*  

	CTRL_REG4 config  

*/
#define BDU							((uint8_t)0x80) //Block data update
#define BLE							((uint8_t)0x40) //Big/little endian data selection
#define HR							((uint8_t)0x08) //High-resolution output mode
//Self-test
#define SELF_TEST_DISABLE			((uint8_t)0x00)	//Self test disable - normal mode
#define SELF_TEST_MODE_1			((uint8_t)0x02) //Self test on mode 1
#define SELF_TEST_MODE_2			((uint8_t)0x04) //Self test on mode 2
//Full-scale selection
#define FS_2                  		((uint8_t)0x00)  /* 2 g  */
#define FS_4                  		((uint8_t)0x10)  /* 4 g  */
#define FS_8                  		((uint8_t)0x20)  /* 8 g  */
#define FS_16                 		((uint8_t)0x30)  /* 16 g */
//SPI serial interface mode selection
#define SIM_4						((uint8_t)0x00) //4-wire interface
#define SIM_3						((uint8_t)0x01) //3-wire interface

/*  

	CTRL_REG5 config  

*/
#define REBOOT_MEMORY				((uint8_t)0x80) //Reboot memory content
#define FIFO_EN						((uint8_t)0x40) //FIFO enable
#define D4D_INT1					((uint8_t)0x04) //4D enable: 4D detection is enabled on INT1 when 6D bit on INT1_CFG is set to 1.
#define D4D_INT2					((uint8_t)0x01) //4D enable: 4D detection is enabled on INT2 pin when 6D bit on INT2_CFG (34h) is set to 1.

/*  

	CTRL_REG6 config  

*/
#define I2_CLICK					((uint8_t)0x80) //Click interrupt on INT2
#define I2_IA1						((uint8_t)0x40) //IA1 interrupt on INT2
#define I2_IA2						((uint8_t)0x20) //IA2 interrupt on INT2
#define I2_BOOT						((uint8_t)0x10) //Enable boot on INT2 pin
#define I2_ACT						((uint8_t)0x08) //Enable activity interrupt on INT2 pin
#define INT_POLARITY				((uint8_t)0x02) //INT1 and INT2 pin polarity (0: active-high; 1: active-low)

/*  

	TEMP_CFG_REG config  

*/
#define TEMP_ENABLE							((uint8_t)0x40) //Temperature sensor (T) enable
#define ADC_ENABLE							((uint8_t)0x80) //ADC enable

/*  

	FIFO_CTRL_REG config  

*/
//FIFO mode selection
#define BYPASS_MODE							((uint8_t)0x00)
#define FIFO_MODE							((uint8_t)0x40)
#define STREAM_MODE							((uint8_t)0x80)
#define STREAM_TO_FIFO						((uint8_t)0xC0)
//Trigger selection
#define TR_INT1								((uint8_t)0x00) //Trigger event allows triggering signal on INT1 
#define TR_INT2								((uint8_t)0x20) //Trigger event allows triggering signal on INT2

//Sensitivity values (Based on Full-Scale)   1 2 4 8 12 16 32 48 64 192 
#define SENSITIVITY_1             1  /* 1 mg/digit*/
#define SENSITIVITY_2             2  /* 2 mg/digit*/
#define SENSITIVITY_4             4  /* 4 mg/digit*/
#define SENSITIVITY_8             8  /* 8 mg/digit*/
#define SENSITIVITY_12            12  /* 12 mg/digit*/
#define SENSITIVITY_16            16  /* 16 mg/digit*/
#define SENSITIVITY_32            32  /* 32 mg/digit*/
#define SENSITIVITY_48            48  /* 48 mg/digit*/
#define SENSITIVITY_64            64  /* 64 mg/digit*/
#define SENSITIVITY_192           192  /* 192 mg/digit*/

//Typedefs
//Operation Modes Configuration struct
typedef enum
{
	HIGH_RESOLUTION = 0,
	NORMAL,
	LOW_POWER
	
}LIS3DH_Operation_Mode;

// Accelerometer Configuration struct
typedef struct 
{
	uint8_t dataRate;
	uint8_t fullScale;
	uint8_t enableAxes;
	bool interruptEnable;
	uint8_t mode;
	
}LIS3DH_InitTypeDef;


// Accelerometer raw data
typedef struct 
{
	uint16_t x;
	uint16_t y;
	uint16_t z;
	
}LIS3DH_DataRaw;

// Accelerometer mg data (scaled data)
typedef struct
{
	uint32_t x;
	uint32_t y;
	uint32_t z;
	
}LIS3DH_DataScaled;

//Functions prototypes
//Private functions
//1. Write IO
void LIS3DH_WriteIO(accConfig acc, uint8_t reg, uint8_t *dataW, uint8_t size);
//2. Read IO
void LIS3DH_ReadIO(accConfig acc, uint8_t reg, uint8_t *dataR, uint8_t size);


//1. Accelerometer initialise function
char* LIS3DH_Init(accConfig acc, LIS3DH_InitTypeDef *accInitDef);
//2. Get Accelerometer raw data
accDataRaw LIS3DH_GetDataRaw(accConfig acc, LIS3DH_Operation_Mode);
//3. Get Accelerometer mg data
LIS3DH_DataScaled LIS3DH_GetDataScaled(accConfig acc, LIS3DH_Operation_Mode);
// Get data filtered
accDataRaw LIS3DH_GetDataFilter(accConfig acc, LIS3DH_Operation_Mode);
//4. Poll for Data Ready
bool LIS3DH_PollDRDY(accConfig acc, uint32_t msTimeout);

//** Calibration functions **//
//1. Set X-Axis calibrate
void LIS3DH_X_calibrate(float x_min, float x_max);
//2. Set Y-Axis calibrate
void LIS3DH_Y_calibrate(float y_min, float y_max);
//3. Set Z-Axis calibrate
void LIS3DH_Z_calibrate(float z_min, float z_max);

#endif
