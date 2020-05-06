
//Header files
#include "LIS3DH.h"
#include <stdio.h>
//SPI Chip Select
#define _LIS3DH_CS_ENBALE		HAL_GPIO_WritePin(LIS3DH_CS_GPIO_Port, LIS3DH_CS_Pin, GPIO_PIN_RESET);
#define _LIS3DH_CS_DISABLE		HAL_GPIO_WritePin(LIS3DH_CS_GPIO_Port, LIS3DH_CS_Pin, GPIO_PIN_SET);


//Library variables
//SPI handle
static SPI_HandleTypeDef accSPI_Handle;
//static uint8_t spiBuffer_XYZ[192]; //Read FIFO Stream - 6 bytes * 32 slots = 192 bytes

static float lis3dh_Sensitivity = SENSITIVITY_1;
//Bias variables
static float X_offset = 0.0f;
static float Y_offset = 0.0f;
static float Z_offset = 0.0f;
//Scaling variables
static float X_Scale = 1.0f;
static float Y_Scale = 1.0f;
static float Z_Scale = 1.0f;
//Filter variables
#define	N	16  //Filter window
//#define	N	40  //Filter window
static float h[N];
static LIS3DH_DataScaled data_in[N];

char buffer [25];

//Functions definitions
//Private functions
//Write IO
void LIS3DH_WriteIO(uint8_t reg, uint8_t *dataW, uint8_t size)
{
	uint8_t data[1 + size];
	data[0] = reg;
	for(int i=1; i<=size; i++)
		data[i]=dataW[i-1];
	//Enable CS
	_LIS3DH_CS_ENBALE;
	//set register value and data
	HAL_SPI_Transmit(&hspi1, data, 1 + size, 1000);
	//Disable CS
	_LIS3DH_CS_DISABLE;
}
//2. Read IO
void LIS3DH_ReadIO(uint8_t reg, uint8_t *dataR, uint8_t size)
{
	uint8_t spiBuf = 0;
	spiBuf = reg | 0x80;
	//Enable CS
	_LIS3DH_CS_ENBALE;
	HAL_Delay(1);
	//set register value
	HAL_SPI_Transmit(&hspi1, &spiBuf, 1, 1000);
	//HAL_Delay(1);
	//Transmit data
	HAL_SPI_Receive(&hspi1, &dataR[0], size, 1000);
	//Disable CS
	HAL_Delay(1);
	_LIS3DH_CS_DISABLE;
	
//	for(uint8_t i=0; i<(size&0x3); i++)
//	{
//		dataR[i] = spiBuf[i];
//	}
}


//1. Accelerometer initialise function
char* LIS3DH_Init(SPI_HandleTypeDef *accSPI, LIS3DH_InitTypeDef *accInitDef)
{
	uint8_t spiData = 0;
	uint8_t spiData_aux = 0;
	
	memcpy(&accSPI_Handle, accSPI, sizeof(*accSPI));
	
	//Enable Axes and Output Data Rate//
	//Set CTRL REG4 settings value
	spiData |= (accInitDef->enableAxes & 0x07);		    //Enable Axes
	spiData |= (accInitDef->dataRate & 0xF0);			//Output Data Rate
	//Write to accelerometer
	LIS3DH_WriteIO(CTRL_REG1, &spiData, 1);
	HAL_Delay(10);
	LIS3DH_ReadIO(CTRL_REG1, &spiData_aux, 1);
	HAL_Delay(10);
	//sprintf(buffer, "%#x, %#x", spiData, spiData_aux);


	//** 2. Full-Scale selection, self test and 4-wire SPI **//
	spiData = 0;
	spiData |= (accInitDef->fullScale & 0x30);	//Full-Scale
	spiData |= BDU;
	if(accInitDef->mode == HIGH_RESOLUTION)
		spiData |= HR;	//High-resolution
	//Write to accelerometer
	LIS3DH_WriteIO(CTRL_REG4, &spiData, 1);
	HAL_Delay(10);
	LIS3DH_ReadIO(CTRL_REG4, &spiData_aux, 1);
	HAL_Delay(10);
	//sprintf(buffer, "%#x, %#x", spiData, spiData_aux);


	//Enable Axes and Output Data Rate//
	//Set CTRL REG4 settings value
	spiData = 0;
	spiData |= (accInitDef->enableAxes & 0x07);		    //Enable Axes
	spiData |= (accInitDef->dataRate & 0xF0);			//Output Data Rate
	//Write to accelerometer
	LIS3DH_WriteIO(CTRL_REG1, &spiData, 1);
	HAL_Delay(10);
	LIS3DH_ReadIO(CTRL_REG1, &spiData_aux, 1);
	HAL_Delay(10);
	//sprintf(buffer, "%#x, %#x", spiData, spiData_aux);

	//FIFO Configuration
//	spiData = 0;
//	spiData |= FIFO_EN;
//	LIS3DH_WriteIO(CTRL_REG5, &spiData, 1);
//	spiData = 0;
//	spiData |= STREAM_MODE;
//	LIS3DH_WriteIO(FIFO_CTRL_REG, &spiData, 1);

	//** 3. Interrupt Configuration **//
	if(accInitDef->interruptEnable)
	{
		spiData = 0x0;
		//spiData |= I1_OVERRUN;
		//Write to accelerometer
		LIS3DH_WriteIO(CTRL_REG3, &spiData, 1);
		LIS3DH_ReadIO(CTRL_REG3, &spiData_aux, 1);
		//sprintf(buffer, "%#x, %#x", spiData, spiData_aux);
	}

	//Assign sensor sensitivity (based on Full-Scale)
	switch(accInitDef->fullScale)
	{
		case FS_2:
			if(accInitDef->mode == HIGH_RESOLUTION)
				lis3dh_Sensitivity = SENSITIVITY_1;
			if(accInitDef->mode == NORMAL)
				lis3dh_Sensitivity = SENSITIVITY_4;
			if(accInitDef->mode == LOW_POWER)
				lis3dh_Sensitivity = SENSITIVITY_16;
			break;
		
		case FS_4:
			if(accInitDef->mode == HIGH_RESOLUTION)
				lis3dh_Sensitivity = SENSITIVITY_2;
			if(accInitDef->mode == NORMAL)
				lis3dh_Sensitivity = SENSITIVITY_8;
			if(accInitDef->mode == LOW_POWER)
				lis3dh_Sensitivity = SENSITIVITY_32;
			break;
		
		case FS_8:
			if(accInitDef->mode == HIGH_RESOLUTION)
				lis3dh_Sensitivity = SENSITIVITY_4;
			if(accInitDef->mode == NORMAL)
				lis3dh_Sensitivity = SENSITIVITY_16;
			if(accInitDef->mode == LOW_POWER)
				lis3dh_Sensitivity = SENSITIVITY_64;
			break;
		
		case FS_16:
			if(accInitDef->mode == HIGH_RESOLUTION)
				lis3dh_Sensitivity = SENSITIVITY_12;
			if(accInitDef->mode == NORMAL)
				lis3dh_Sensitivity = SENSITIVITY_48;
			if(accInitDef->mode == LOW_POWER)
				lis3dh_Sensitivity = SENSITIVITY_192;
			break;
	}
	//_LIS3DH_CS_DISABLE;

	//Filter impulse response
	for	(int j=0; j<N; j++)	h[j]=1.0/N;

	return buffer;
}
//2. Get Accelerometer raw data
LIS3DH_DataRaw LIS3DH_GetDataRaw(LIS3DH_Operation_Mode mode)
{
	uint8_t spiBuf[2];
	LIS3DH_DataRaw tempDataRaw = {0};
	uint8_t numBits = 0;
	uint8_t UINT16_LEN = 16;
	if(mode == LOW_POWER) numBits = 8; //not implemented yet
	if(mode == NORMAL) numBits = 10;
	if(mode == HIGH_RESOLUTION) numBits = 12;

	//Read X data
	LIS3DH_ReadIO(OUT_X_L, spiBuf, 2);
	/* x = (MSB<<8) + LSB */
	tempDataRaw.x = ((spiBuf[1] << 8) + spiBuf[0]);
	/* Shift from left-justified to right-justified */
//	tempDataRaw.x >>= (UINT16_LEN-numBits);
//	/* Need to handle negative number */
//	if((tempDataRaw.x  & ( 0x0001<<(numBits-1) )) ==  0x0001<<(numBits-1)  )
//	{
//			tempDataRaw.x  = ~tempDataRaw.x ;            //invert bits
//			tempDataRaw.x  &= ( 0xFFFF>>(16-numBits) );  //but keep just the numBits
//			tempDataRaw.x  = ~tempDataRaw.x ;            //invert bits
//    }

	//Read Y data
	LIS3DH_ReadIO(OUT_Y_L, spiBuf, 2);
	tempDataRaw.y = ((spiBuf[1] << 8) + spiBuf[0]);
//	tempDataRaw.y >>= (UINT16_LEN-numBits); //Shift from left-justified to right-justified
//	if((tempDataRaw.y  & ( 0x0001<<(numBits-1) )) ==  0x0001<<(numBits-1)  ) //Need to handle negative number
//	    {
//			tempDataRaw.y  = ~tempDataRaw.x ;            //invert bits
//			tempDataRaw.y  &= ( 0xFFFF>>(16-numBits) );  //but keep just the 10-bits
//			tempDataRaw.y  = ~tempDataRaw.x ;            //invert bits
//	    }
	
	//Read Z data
	LIS3DH_ReadIO(OUT_Z_L, spiBuf, 2);
	tempDataRaw.z = ((spiBuf[1] << 8) + spiBuf[0]);
//	tempDataRaw.z >>= (UINT16_LEN-numBits); //Shift from left-justified to right-justified
//	if((tempDataRaw.z  & ( 0x0001<<(numBits-1) )) ==  0x0001<<(numBits-1)  ) //Need to handle negative number
//	    {
//			tempDataRaw.z  = ~tempDataRaw.x ;            //invert bits
//			tempDataRaw.z  &= ( 0xFFFF>>(16-numBits) );  //but keep just the 10-bits
//			tempDataRaw.z  = ~tempDataRaw.x ;            //invert bits
//	    }
	
	return tempDataRaw;
	
}
//3. Get Accelerometer mg data
LIS3DH_DataScaled LIS3DH_GetDataScaled(LIS3DH_Operation_Mode mode)
{
	//Read raw data
	LIS3DH_DataRaw tempRawData;
	if(mode == LOW_POWER) tempRawData = LIS3DH_GetDataRaw(LOW_POWER);
	if(mode == NORMAL) tempRawData = LIS3DH_GetDataRaw(NORMAL);
	if(mode == HIGH_RESOLUTION) tempRawData = LIS3DH_GetDataRaw(HIGH_RESOLUTION);

	//Scale data and return 
	LIS3DH_DataScaled tempScaledData;
	tempScaledData.x = ((tempRawData.x - X_offset) * lis3dh_Sensitivity * X_Scale);
	tempScaledData.y = ((tempRawData.y - Y_offset) * lis3dh_Sensitivity * Y_Scale);
	tempScaledData.z = ((tempRawData.z - Z_offset) * lis3dh_Sensitivity * Z_Scale);
	
	return tempScaledData;
}

LIS3DH_DataRaw LIS3DH_GetDataFilter(LIS3DH_Operation_Mode mode){

	 LIS3DH_DataRaw	left_out_sample = {0};
	 LIS3DH_DataRaw	left_in_sample = {0};
	 LIS3DH_DataScaled data_out = {0};
	 int i;


	//Get data from ACC
	left_in_sample = LIS3DH_GetDataRaw(mode);

	//X - filter
	data_in[0].x = (uint32_t)left_in_sample.x;
	for	(i=0;i<N;i++) data_out.x += h[i]*data_in[i].x;
	left_out_sample.x	= (uint16_t)(data_out.x);
	data_out.x = 0;
	//Y - filter
	data_in[0].y = (uint32_t)left_in_sample.y;
	for	(i=0;i<N;i++) data_out.y += h[i]*data_in[i].y;
	left_out_sample.y	= (uint16_t)(data_out.y);
	data_out.y = 0;
	//Z - filter
	data_in[0].z = (uint32_t)left_in_sample.z;
	for	(i=0;i<N;i++) data_out.z += h[i]*data_in[i].z;
	left_out_sample.z	= (uint16_t)(data_out.z);
	data_out.z = 0;

	//Move filter buffer - last out
	for	(i=(N-1); i>0 ; i--) data_in[i] = data_in[i-1];

	return left_out_sample;
}
//4. Poll for Data Ready
bool LIS3DH_PollDRDY(uint32_t msTimeout)
{
	uint8_t Acc_status;
	uint32_t startTick = HAL_GetTick();
	do
	{
		//Read status register with a timeout
		LIS3DH_ReadIO(STATUS_REG, &Acc_status, 1);
		if(Acc_status & 0x08)break;  //XYZ data available
		
	}while((Acc_status & 0x08)==0 && (HAL_GetTick() - startTick) < msTimeout);
	if(Acc_status & 0x08)
	{
		return true;
	}
	return false;
	
}

//** Calibration functions **//
//1. Set X-Axis calibrate
void LIS3DH_X_calibrate(float x_min, float x_max)
{
	X_offset = (x_max+x_min)/2.0f;
	//X_Scale = 1/(x_max - X_offset);
}
//2. Set Y-Axis calibrate
void LIS3DH_Y_calibrate(float y_min, float y_max)
{
	Y_offset = (y_max+y_min)/2.0f;
	//Y_Scale = 1/(y_max - Y_offset);
}
//3. Set Z-Axis calibrate
void LIS3DH_Z_calibrate(float z_min, float z_max)
{
	Z_offset = (z_max+z_min)/2.0f;
	//Z_Scale = 1/(z_max - Z_offset);
}

