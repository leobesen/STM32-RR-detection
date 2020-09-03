/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************

  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "rf.h"
#include "rtc.h"
#include "spi.h"
#include "app_entry.h"
#include "app_common.h"
#include "tim.h"
#include "usart.h"
#include "usb.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "LIS3DH.h"
#include "LIS3DH.h"
#include "MPU9250.h"
#include "MAX30100.h"
#include "app_conf.h"
#include "app_ble.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BLE_PACK_ID 0x28
#define BLE_FSR_PACK 0x01
#define BLE_ACC_PACK 0x02
#define BLE_PPG_PACK 0x03
#define ADS1115_ADDRESS 0x48
uint8_t ADSwrite[3];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void read_acc(void);
void read_fsr(void);
void read_ppg(void);

void prepare_acc_ble_pack(uint8_t* buff);
void prepare_fsr_ble_pack(uint8_t* buff);
void prepare_ppg_ble_pack(uint8_t* buff);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

LIS3DH_InitTypeDef myAccConfigDef = {
		DATARATE_400,
		FS_2,
		XYZ_ENABLE,
		false,
		HIGH_RESOLUTION
};

accConfig acc1_1= {
		&hspi1,
		SPI1_CS_1_GPIO_Port,
		SPI1_CS_1_Pin
};
accConfig acc1_2 = {
		&hspi1,
		SPI1_CS_2_GPIO_Port,
		SPI1_CS_2_Pin
};
accConfig acc2_1 = {
		&hspi2,
		SPI2_CS_1_GPIO_Port,
		SPI2_CS_1_Pin
};
accConfig acc2_2 = {
		&hspi2,
		SPI2_CS_2_GPIO_Port,
		SPI2_CS_2_Pin
};

accDataRaw acc_data_raw[4];

uint8_t WAI_LIS3DH_1 = 0;
uint8_t WAI_LIS3DH_2 = 0;
uint8_t WAI_MPU9250 = 0;

uint8_t reg = MAX30100_PART_ID;
uint8_t ID_MAX30100 = 0;
uint8_t status = 0;

int16_t rawData_MPU9250[3] = {0};

uint16_t IR = 0, RED = 0;

// buffers for 1 second of storage
uint8_t ble_buff_PPG[2][200] = {0};
uint8_t ble_buff_ACC[4][120] = {0};
uint8_t ble_buff_FSR[20] = {0};

char flag_reading_completed = 0x01;

uint8_t prescaler_acc = 0;
uint8_t prescaler_fsr = 0;
uint8_t prescaler_send_pack = 0;

uint8_t counter_acc = 0;
uint8_t counter_ppg = 0;
uint8_t counter_fsr = 0;

char acc_pack_number = 0x00;
char ppg_pack_number = 0x00;

char flag_fsr_buff_full = 0x00;

char buffer_select = 0x00;



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USB_PCD_Init();
  MX_RF_Init();
  MX_RTC_Init();
  MX_I2C3_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM16_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  //
  //ACC Config

  /*
   * LIS3DH
   * */
  LIS3DH_ReadIO(acc1_1, WHO_AM_I, &WAI_LIS3DH_1, 1);
  LIS3DH_Init(acc1_1, &myAccConfigDef);

  LIS3DH_ReadIO(acc1_2, WHO_AM_I, &WAI_LIS3DH_2, 1);
  LIS3DH_Init(acc1_2, &myAccConfigDef);

  /*
   * MPU9250
   * */

   readByte(acc2_1, WHO_AM_I_MPU9250, &WAI_MPU9250, 1);

   initMPU9250(acc2_1);

  /*
   * MAX30100
   * */

   read_byte(reg, &ID_MAX30100);

   begin();

   UTIL_SEQ_RegTask( 1<< READ_ACC_TASK, UTIL_SEQ_RFU, read_acc );
   UTIL_SEQ_RegTask( 1<< READ_FSR_TASK, UTIL_SEQ_RFU, read_fsr );
   UTIL_SEQ_RegTask( 1<< READ_PPG_TASK, UTIL_SEQ_RFU, read_ppg );

  /* USER CODE END 2 */

  /* Init code for STM32_WPAN */  
  APPE_Init();

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  UTIL_SEQ_Run(UTIL_SEQ_DEFAULT);


  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Macro to configure the PLL multiplication factor 
  */
  __HAL_RCC_PLL_PLLM_CONFIG(RCC_PLLM_DIV1);
  /** Macro to configure the PLL clock source 
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_MSI);
  /** Configure LSE Drive Capability 
  */
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK4|RCC_CLOCKTYPE_HCLK2
                              |RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK4Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks 
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SMPS|RCC_PERIPHCLK_RFWAKEUP
                              |RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C3
                              |RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.PLLSAI1.PLLN = 24;
  PeriphClkInitStruct.PLLSAI1.PLLP = RCC_PLLP_DIV2;
  PeriphClkInitStruct.PLLSAI1.PLLQ = RCC_PLLQ_DIV2;
  PeriphClkInitStruct.PLLSAI1.PLLR = RCC_PLLR_DIV2;
  PeriphClkInitStruct.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_USBCLK;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInitStruct.RFWakeUpClockSelection = RCC_RFWKPCLKSOURCE_LSE;
  PeriphClkInitStruct.SmpsClockSelection = RCC_SMPSCLKSOURCE_HSE;
  PeriphClkInitStruct.SmpsDivSelection = RCC_SMPSCLKDIV_RANGE0;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN Smps */

  /* USER CODE END Smps */
  /** Enable MSI Auto calibration 
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/* USER CODE BEGIN 4 */
void read_fsr(void){
	/*
	 * ADS1115
	 * */
	ADSwrite[0] = 0x01;
	ADSwrite[1] = 0xC1;// 11000001
	ADSwrite[2] = 0x83; // 10000011
	HAL_I2C_Master_Transmit(&hi2c3, ADS1115_ADDRESS<<1,ADSwrite,3,100);
	ADSwrite[0] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c3, ADS1115_ADDRESS<<1,ADSwrite,1,100);
	HAL_Delay(1);
	HAL_I2C_Master_Receive(&hi2c3, ADS1115_ADDRESS<<1,ADSwrite,2,100);

	ble_buff_FSR[0+counter_fsr] = ADSwrite[1];
	ble_buff_FSR[1+counter_fsr] = ADSwrite[0];

	if(counter_fsr == 18){
		counter_fsr=0;
		UTIL_SEQ_SetTask(1 << SEND_FSR_DATA_BLE_TASK, CFG_SCH_PRIO_1);
	}else
		counter_fsr+=2;
	/*
	 * BUFFER --> [0:19]
	 * */
	//HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
}

void read_acc(void){

	acc_data_raw[0] = LIS3DH_GetDataRaw(acc1_1, HIGH_RESOLUTION);
	acc_data_raw[1] = LIS3DH_GetDataRaw(acc1_2, HIGH_RESOLUTION);
	acc_data_raw[2] = readAccelData(acc2_1);

	for(int i=0;i<4;i++){
		//X
		ble_buff_ACC[i][0+counter_acc] = (acc_data_raw[i].x & 0x00FF);
		ble_buff_ACC[i][1+counter_acc] = ((acc_data_raw[i].x >> 8) & 0x00FF);
		//Y
		ble_buff_ACC[i][2+counter_acc] = (acc_data_raw[i].y & 0x00FF);
		ble_buff_ACC[i][3+counter_acc] = ((acc_data_raw[i].y >> 8) & 0x00FF);
		//Z
		ble_buff_ACC[i][4+counter_acc] = (acc_data_raw[i].z & 0x00FF);
		ble_buff_ACC[i][5+counter_acc] = ((acc_data_raw[i].z >> 8) & 0x00FF);
	}
	counter_acc+=6;

	if(counter_acc == 30){
		// send first pack
		acc_pack_number = 0x01;
		UTIL_SEQ_SetTask(1 << SEND_ACC_DATA_BLE_TASK, CFG_SCH_PRIO_1);
	}
	if(counter_acc == 60){
		// send second pack
		acc_pack_number = 0x02;
		UTIL_SEQ_SetTask(1 << SEND_ACC_DATA_BLE_TASK, CFG_SCH_PRIO_1);
	}
	if(counter_acc == 90){
		// send third pack
		acc_pack_number = 0x03;
		UTIL_SEQ_SetTask(1 << SEND_ACC_DATA_BLE_TASK, CFG_SCH_PRIO_1);
	}
	if(counter_acc == 120){
		// send fourth pack
		acc_pack_number = 0x04;
		UTIL_SEQ_SetTask(1 << SEND_ACC_DATA_BLE_TASK, CFG_SCH_PRIO_1);
	}

	if(counter_acc == 120)
		counter_acc = 0;

}


void read_ppg(void){
	/*
	 * MAX30100
	 * */
	readFIFO(&RED, &IR);

	ble_buff_PPG[0][0+counter_ppg] = (RED & 0x00FF);
	ble_buff_PPG[0][1+counter_ppg] = ((RED >> 8) & 0x00FF);

	ble_buff_PPG[1][0+counter_ppg] = (IR & 0x00FF);
	ble_buff_PPG[1][1+counter_ppg] = ((IR >> 8) & 0x00FF);

	counter_ppg+=2;

	if(counter_ppg == 50){
		// send first pack
		ppg_pack_number = 0x01;
		UTIL_SEQ_SetTask(1 << SEND_PPG_DATA_BLE_TASK, CFG_SCH_PRIO_1);
	}
	if(counter_ppg == 100){
		// send second pack
		ppg_pack_number = 0x02;
		UTIL_SEQ_SetTask(1 << SEND_PPG_DATA_BLE_TASK, CFG_SCH_PRIO_1);
	}
	if(counter_ppg == 150){
		// send third pack
		ppg_pack_number = 0x03;
		UTIL_SEQ_SetTask(1 << SEND_PPG_DATA_BLE_TASK, CFG_SCH_PRIO_1);
	}
	if(counter_ppg == 200){
		// send fourth pack
		ppg_pack_number = 0x04;
		UTIL_SEQ_SetTask(1 << SEND_PPG_DATA_BLE_TASK, CFG_SCH_PRIO_1);
	}

	if(counter_ppg == 200)
		counter_ppg = 0;

}

void prepare_fsr_ble_pack(uint8_t* buff){

	buff[0] = BLE_PACK_ID;
	buff[1] = BLE_FSR_PACK;
	buff[2] = 0x01;
	for(int i=3;i<23;i++)
		buff[i] = ble_buff_FSR[i-3];

}

void prepare_acc_ble_pack(uint8_t* buff){

	uint8_t acc_buffer[123] = {0};

	if(acc_pack_number != 0x00){
		switch(acc_pack_number){
			case 0x01:
				acc_buffer[0] = BLE_PACK_ID;
				acc_buffer[1] = BLE_ACC_PACK;
				acc_buffer[2] = acc_pack_number;
				for(int i=0;i<30;i++){
					acc_buffer[3 + i] = ble_buff_ACC[0][i];
					acc_buffer[33 + i] = ble_buff_ACC[1][i];
					acc_buffer[63 + i] = ble_buff_ACC[2][i];
					acc_buffer[93 + i] = ble_buff_ACC[3][i];
				}
				break;
			case 0x02:
				acc_buffer[0] = BLE_PACK_ID;
				acc_buffer[1] = BLE_ACC_PACK;
				acc_buffer[2] = acc_pack_number;
				for(int i=30;i<60;i++){
					acc_buffer[3 + i - 30] = ble_buff_ACC[0][i];
					acc_buffer[33 + i- 30] = ble_buff_ACC[1][i];
					acc_buffer[63 + i- 30] = ble_buff_ACC[2][i];
					acc_buffer[93 + i - 30] = ble_buff_ACC[3][i];
				}
				break;
			case 0x03:
				acc_buffer[0] = BLE_PACK_ID;
				acc_buffer[1] = BLE_ACC_PACK;
				acc_buffer[2] = acc_pack_number;
				for(int i=60;i<90;i++){
					acc_buffer[3 + i - 60] = ble_buff_ACC[0][i];
					acc_buffer[33 + i - 60] = ble_buff_ACC[1][i];
					acc_buffer[63 + i - 60] = ble_buff_ACC[2][i];
					acc_buffer[93 + i - 60] = ble_buff_ACC[3][i];
				}
				break;
			case 0x04:
				acc_buffer[0] = BLE_PACK_ID;
				acc_buffer[1] = BLE_ACC_PACK;
				acc_buffer[2] = acc_pack_number;
				for(int i=90;i<120;i++){
					acc_buffer[3 + i - 90] = ble_buff_ACC[0][i];
					acc_buffer[33 + i - 90] = ble_buff_ACC[1][i];
					acc_buffer[63 + i - 90] = ble_buff_ACC[2][i];
					acc_buffer[93 + i - 90] = ble_buff_ACC[3][i];
				}
				break;
			default:
				acc_pack_number = 0x00;
				break;
		}
		acc_pack_number = 0x00;
		for(int i=0;i<123;i++)
			buff[i] = acc_buffer[i];
	}
}
void prepare_ppg_ble_pack(uint8_t* buff){

	uint8_t ppg_buffer[103] = {0};

	if(ppg_pack_number != 0x00){
		switch(ppg_pack_number){
			case 0x01:
				ppg_buffer[0] = BLE_PACK_ID;
				ppg_buffer[1] = BLE_PPG_PACK;
				ppg_buffer[2] = ppg_pack_number;
				for(int i=0;i<50;i++){
					ppg_buffer[3 + i] = ble_buff_PPG[0][i];
					ppg_buffer[53 + i] = ble_buff_PPG[1][i];
				}
				break;
			case 0x02:
				ppg_buffer[0] = BLE_PACK_ID;
				ppg_buffer[1] = BLE_PPG_PACK;
				ppg_buffer[2] = ppg_pack_number;
				for(int i=50;i<100;i++){
					ppg_buffer[3 + i - 50] = ble_buff_PPG[0][i];
					ppg_buffer[53 + i - 50] = ble_buff_PPG[1][i];
				}
				break;
			case 0x03:
				ppg_buffer[0] = BLE_PACK_ID;
				ppg_buffer[1] = BLE_PPG_PACK;
				ppg_buffer[2] = ppg_pack_number;
				for(int i=100;i<150;i++){
					ppg_buffer[3 + i - 100] = ble_buff_PPG[0][i];
					ppg_buffer[53 + i - 100] = ble_buff_PPG[1][i];
				}
				break;
			case 0x04:
				ppg_buffer[0] = BLE_PACK_ID;
				ppg_buffer[1] = BLE_PPG_PACK;
				ppg_buffer[2] = ppg_pack_number;
				for(int i=150;i<200;i++){
					ppg_buffer[3 + i - 150] = ble_buff_PPG[0][i];
					ppg_buffer[53 + i - 150] = ble_buff_PPG[1][i];
				}
				break;
			default:
				ppg_pack_number = 0x00;
				break;
		}
		ppg_pack_number = 0x00;
		for(int i=0;i<103;i++)
			buff[i] = ppg_buffer[i];
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  if(htim == &htim16){
	  if(APP_BLE_Get_Server_Connection_Status() == APP_BLE_CONNECTED_SERVER){
		  // Increment prescaler for fsr and acc
		  prescaler_fsr++;
		  prescaler_acc++;
		  prescaler_send_pack++;
		  // read ppg sensor - 100Hz
		  UTIL_SEQ_SetTask(1 << READ_PPG_TASK, CFG_SCH_PRIO_0);
		  // read fsr sensor - 10Hz
		  if(prescaler_fsr == 10){
			  prescaler_fsr = 0;
			  UTIL_SEQ_SetTask(1 << READ_FSR_TASK, CFG_SCH_PRIO_0);
		  }
		  // read acc sensor - 20Hz
		  if(prescaler_acc == 5){
			  prescaler_acc = 0;
			  UTIL_SEQ_SetTask(1 << READ_ACC_TASK, CFG_SCH_PRIO_0);
		  }
	  }

  }

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_TIM_PeriodElapsedCallback could be implemented in the user file
   */
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
