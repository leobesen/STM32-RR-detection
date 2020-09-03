/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
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
#include "rf.h"
#include "rtc.h"
#include "app_entry.h"
#include "app_common.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_ble.h"
#include "stm32_seq.h"
#include <stdio.h>
#include <string.h>
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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern char VCPInitialized; // indica interface USB pronta
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void transmit_CDC_messege(uint8_t*, uint16_t);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_RF_Init();
  MX_RTC_Init();
  MX_USB_Device_Init();
  /* USER CODE BEGIN 2 */

  while(!VCPInitialized) {
	  HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin); // led red pisca se USB off
	  HAL_Delay(500);
  }
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);

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

  /** Configure LSE Drive Capability 
  */
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI
                              |RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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
                              |RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInitStruct.RFWakeUpClockSelection = RCC_RFWKPCLKSOURCE_LSE;
  PeriphClkInitStruct.SmpsClockSelection = RCC_SMPSCLKSOURCE_HSE;
  PeriphClkInitStruct.SmpsDivSelection = RCC_SMPSCLKDIV_RANGE1;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN Smps */

  /* USER CODE END Smps */
}

/* USER CODE BEGIN 4 */
void transmit_CDC_messege(uint8_t* Buff, uint16_t Len){

	uint8_t buffer_receive[PAYLOAD_LENGTH-3] = {0};

	uint8_t result = 0;
	char t[800];
	uint8_t t_i[800];
	char aux[12];
	uint16_t dest[60] = {0};
	unsigned size;

	memset(t,0,800);
	memset(t_i,0,800);
	memset(aux,0,10);

	for(int i=0;i<PAYLOAD_LENGTH-3;i++)
		Buff[i+3] = buffer_receive[i];

	/*
	 * 	FSR - 10 samples
	 * */
	if((Buff[0]==BLE_PACK_ID)&&(Buff[1]==BLE_FSR_PACK)&&(Buff[2]==0x01)){
		for(int i=0; i<10; i++){
			dest[i] = (uint16_t)(((uint16_t)buffer_receive[(2*i+1)] << 8) | buffer_receive[2*i]);  // Turn the MSB and LSB into a signed 16-bit value
		}
		sprintf(aux,"28,%u,%u,",Buff[1],Buff[2]);
		strcat(t,aux);
		memset(aux,0,10);
		for(int i=0;i<9;i++){
			sprintf(aux,"%u,",dest[i]);
			strcat(t,aux);
			memset(aux,0,10);
		}
		sprintf(aux,"%u\n",dest[9]);
		strcat(t,aux);
	}
	/*
	 * 	ACC - 60 samples - 15 samples of each acc
	 * */
	if((Buff[0]==BLE_PACK_ID)&&(Buff[1]==BLE_ACC_PACK)){
		for(int i=0; i<60; i++){
			dest[i] = (uint16_t)(((uint16_t)buffer_receive[(2*i+1)] << 8) | buffer_receive[2*i]);  // Turn the MSB and LSB into a signed 16-bit value
		}
		sprintf(aux,"28,%u,%u,",Buff[1],Buff[2]);
		strcat(t,aux);
		memset(aux,0,10);
		for(int i=0;i<59;i++){
			sprintf(aux,"%u,",dest[i]);
			strcat(t,aux);
			memset(aux,0,10);
		}
		sprintf(aux,"%u\n",dest[59]);
		strcat(t,aux);
	}
	/*
	 * 	PPG - 50 samples - 25 samples of each led
	 * */
	if((Buff[0]==BLE_PACK_ID)&&(Buff[1]==BLE_PPG_PACK)){
		for(int i=0; i<50; i++){
			dest[i] = (uint16_t)(((uint16_t)buffer_receive[(2*i+1)] << 8) | buffer_receive[2*i]);  // Turn the MSB and LSB into a signed 16-bit value
		}
		sprintf(aux,"28,%u,%u,",Buff[1],Buff[2]);
		strcat(t,aux);
		memset(aux,0,10);
		for(int i=0;i<49;i++){
			sprintf(aux,"%u,",dest[i]);
			strcat(t,aux);
			memset(aux,0,10);
		}
		sprintf(aux,"%u\n",dest[49]);
		strcat(t,aux);
	}

	for(int i=0;i<strlen(t);i++){
		t_i[i] = (uint8_t)t[i];
	}

	size = (unsigned)strlen(t_i);
	result = CDC_Transmit_FS(t_i, size);


	if (result == 1)
		HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);


}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  if(GPIO_Pin == SW1_Pin){
//	  if(P2P_Client_APP_Get_State () != APP_BLE_CONNECTED_CLIENT)
//		{
//		  UTIL_SEQ_SetTask(1 << CFG_TASK_START_SCAN_ID, CFG_SCH_PRIO_0);
//		  HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
//		}else{
//			HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin);
//		}
  }
  if(GPIO_Pin == SW2_Pin){}
  if(GPIO_Pin == SW3_Pin){}
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
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
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
	while(1){
		__NOP();
	}
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
