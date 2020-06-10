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
#include "LIS3DH_I2C.h"
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

void get_buffer_values(uint8_t* buff);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//LIS3DH_InitTypeDef myAccConfigDef = {
//		DATARATE_400,
//		FS_2,
//		XYZ_ENABLE,
//		false,
//		HIGH_RESOLUTION
//};

uint8_t WAI_LIS3DH = 0;
uint8_t WAI_MPU9250 = 0;

uint8_t reg = MAX30100_PART_ID;
uint8_t ID_MAX30100 = 0;
uint8_t status = 0;

int16_t rawData_MPU9250[3] = {0};

uint16_t IR = 0, RED = 0;

uint8_t ble_buff_PPG[200] = {0};
uint8_t ble_buff_ACC_FSR[140] = {0};

char flag_reading_completed = 0x01;

uint8_t prescaler_acc = 0;
uint8_t prescaler_fsr = 0;
uint8_t prescaler_send_pack = 0;

uint8_t counter_acc = 0;
uint8_t counter_ppg = 0;
uint8_t counter_fsr = 0;

char flag_ppg_buff_full = 0x00;
char flag_acc_buff_full = 0x00;
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
  //WAI_LIS3DH = read_id();
  //begin_LIS3DH_I2C(LIS3DH_G_CHIP_ADDR, LIS3DH_DR_NR_LP_400HZ, LIS3DH_FS_2G);

  /*
   * MPU9250
   * */

   readByte(WHO_AM_I_MPU9250, &WAI_MPU9250, 1);

   initMPU9250();

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
	if(flag_fsr_buff_full == 0x00){
		ADSwrite[0] = 0x01;
		ADSwrite[1] = 0xC1;// 11000001
		ADSwrite[2] = 0x83; // 10000011
		HAL_I2C_Master_Transmit(&hi2c3, ADS1115_ADDRESS<<1,ADSwrite,3,100);
		ADSwrite[0] = 0x00;
		HAL_I2C_Master_Transmit(&hi2c3, ADS1115_ADDRESS<<1,ADSwrite,1,100);
		HAL_Delay(1);
		HAL_I2C_Master_Receive(&hi2c3, ADS1115_ADDRESS<<1,ADSwrite,2,100);

		ble_buff_ACC_FSR[0+counter_fsr] = ADSwrite[1];
		ble_buff_ACC_FSR[1+counter_fsr] = ADSwrite[0];

		if(counter_fsr == 18){
			counter_fsr=0;
			flag_fsr_buff_full = 0x01;
		}else
			counter_fsr+=2;
	}
	/*
	 * BUFFER --> [0:19]
	 * */
	//HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
}

void read_acc(void){
	/*
	 * MPU9250
	 * */
	if(flag_acc_buff_full == 0x00){
		readAccelData(rawData_MPU9250);
		//X
		ble_buff_ACC_FSR[20+counter_acc] = (rawData_MPU9250[0] & 0x00FF);
		ble_buff_ACC_FSR[21+counter_acc] = ((rawData_MPU9250[0] >> 8) & 0x00FF);
		//Y
		ble_buff_ACC_FSR[60+counter_acc] = (rawData_MPU9250[1] & 0x00FF);
		ble_buff_ACC_FSR[61+counter_acc] = ((rawData_MPU9250[1] >> 8) & 0x00FF);
		//Z
		ble_buff_ACC_FSR[100+counter_acc] = (rawData_MPU9250[2] & 0x00FF);
		ble_buff_ACC_FSR[101+counter_acc] = ((rawData_MPU9250[2] >> 8) & 0x00FF);

		if(counter_acc == 38){
			counter_acc = 0;
			flag_acc_buff_full = 0x01;
		}else
			counter_acc+=2;
	}
	/*
	 * BUFFER --> [20:139]
	 * */
}


void read_ppg(void){
	/*
	 * MAX30100
	 * */
	if(flag_ppg_buff_full == 0){
		readFIFO(&RED, &IR);

		//ble_buffer[140+counter_ppg] = (RED & 0x00FF);
		//ble_buffer[141+counter_ppg] = ((RED >> 8) & 0x00FF);

		ble_buff_PPG[0+counter_ppg] = (IR & 0x00FF);
		ble_buff_PPG[1+counter_ppg] = ((IR >> 8) & 0x00FF);

		if(counter_ppg == 198){
			counter_ppg = 0;
			flag_ppg_buff_full = 0x01;
		}else
			counter_ppg+=2;
	}
	/*
	 * BUFFER --> [140:539]
	 * */
	//HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
}

void get_buffer_values(uint8_t* buff){

	/*
	 * buffer_select
	 * 0 --> ACC and FSR - 140 Bytes
	 * 1 --> PPG - 200 Bytes
	 * */
	if(buffer_select == 0x00){
		buffer_select = 0x01; //select buffer for the next transmission
		// preamble for acc and fsr data
		buff[0] = 0x00;
		buff[1] = 0xFA;
		buff[2] = 0xAF;
		buff[3] = 0x00;
		// buffer fulfill
		for(int i = 0; i < 140; i++)
				buff[i+4] = ble_buff_ACC_FSR[i];
		// enable read
		flag_acc_buff_full = 0x00;
		flag_fsr_buff_full = 0x00;
	}else{
		buffer_select = 0x00; //select buffer for the next transmission
		// preamble for ppg data
		buff[0] = 0x00;
		buff[1] = 0xAF;
		buff[2] = 0xFA;
		buff[3] = 0x00;
		// buffer fulfill
		for(int i = 0; i < 200; i++)
				buff[i+4] = ble_buff_PPG[i];
		// enable read
		flag_ppg_buff_full = 0x00;
	}
	HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
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
		  if(prescaler_send_pack == 50){
			  UTIL_SEQ_SetTask(1 << CFG_MY_TASK_NOTIFY_DATA, CFG_SCH_PRIO_0);
			  prescaler_send_pack = 0;
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
