/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : p2p_server_app.c
 * Description        : P2P Server Application
 ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "p2p_server_app.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tim.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  uint16_t X;
  uint16_t Y;
  uint16_t Z;
} P2P_ACC_CharValue_t;

typedef struct
{
  uint16_t ecg_value;
} P2P_ECG_CharValue_t;

typedef struct
{
  uint16_t ppg_value;
} P2P_PPG_CharValue_t;

typedef struct
{
  uint8_t  NotificationStatus;
  uint16_t Parameter;
  P2P_ACC_CharValue_t  ACC;
  P2P_ECG_CharValue_t	ECG;
  P2P_PPG_CharValue_t  PPG;
  uint8_t  Update_timer_Id;
} P2P_Server_App_Context_t;
/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/*								BLE Frequency Update								 */
#define DATA_CHANGE_PERIOD        (0.1*1000*1000/(5*CFG_TS_TICK_VAL)) /*25ms - 40Hz*/
//#define DATA_CHANGE_PERIOD        (0.1*1000*1000/(CFG_TS_TICK_VAL)) /*100ms - 10Hz */
//#define DATA_CHANGE_PERIOD        (0.1*1000*1000/(20*CFG_TS_TICK_VAL)) /*5ms - 200Hz */
//#define DATA_CHANGE_PERIOD        (0.1*1000*1000/(40*CFG_TS_TICK_VAL)) /*5ms - 200Hz */
/*-----------------------------------------------------------------------------------*/
/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
PLACE_IN_SECTION("BLE_APP_CONTEXT") static P2P_Server_App_Context_t P2P_Server_App_Context;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void P2PS_Send_Notification_Task_FSR(void);
static void P2PS_Send_Notification_Task_ACC(void);
static void P2PS_Send_Notification_Task_PPG(void);
static void P2PS_APP_context_Init(void);
static void P2PS_DataChange_Timer_Callback(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void P2PS_STM_App_Notification(P2PS_STM_App_Notification_evt_t *pNotification)
{
/* USER CODE BEGIN P2PS_STM_App_Notification_1 */

/* USER CODE END P2PS_STM_App_Notification_1 */
  switch(pNotification->P2P_Evt_Opcode)
  {
/* USER CODE BEGIN P2PS_STM_App_Notification_P2P_Evt_Opcode */

/* USER CODE END P2PS_STM_App_Notification_P2P_Evt_Opcode */

    case P2PS_STM__NOTIFY_ENABLED_EVT:
/* USER CODE BEGIN P2PS_STM__NOTIFY_ENABLED_EVT */
    	P2P_Server_App_Context.NotificationStatus = 1;
    	HAL_TIM_Base_Start_IT(&htim16);
    	/* Start the timer used to update the characteristic */
    	//HW_TS_Start(P2P_Server_App_Context.Update_timer_Id, DATA_CHANGE_PERIOD);

/* USER CODE END P2PS_STM__NOTIFY_ENABLED_EVT */
      break;

    case P2PS_STM_NOTIFY_DISABLED_EVT:
/* USER CODE BEGIN P2PS_STM_NOTIFY_DISABLED_EVT */
    	P2P_Server_App_Context.NotificationStatus = 0;
    	//HW_TS_Stop(P2P_Server_App_Context.Update_timer_Id);
/* USER CODE END P2PS_STM_NOTIFY_DISABLED_EVT */
      break;

    case P2PS_STM_WRITE_EVT:
/* USER CODE BEGIN P2PS_STM_WRITE_EVT */


/* USER CODE END P2PS_STM_WRITE_EVT */
      break;

    default:
/* USER CODE BEGIN P2PS_STM_App_Notification_default */

/* USER CODE END P2PS_STM_App_Notification_default */
      break;
  }
/* USER CODE BEGIN P2PS_STM_App_Notification_2 */

/* USER CODE END P2PS_STM_App_Notification_2 */
  return;
}

void P2PS_APP_Notification(P2PS_APP_ConnHandle_Not_evt_t *pNotification)
{
/* USER CODE BEGIN P2PS_APP_Notification_1 */

/* USER CODE END P2PS_APP_Notification_1 */
  switch(pNotification->P2P_Evt_Opcode)
  {
/* USER CODE BEGIN P2PS_APP_Notification_P2P_Evt_Opcode */

/* USER CODE END P2PS_APP_Notification_P2P_Evt_Opcode */
  case PEER_CONN_HANDLE_EVT :
/* USER CODE BEGIN PEER_CONN_HANDLE_EVT */

/* USER CODE END PEER_CONN_HANDLE_EVT */
    break;

    case PEER_DISCON_HANDLE_EVT :
/* USER CODE BEGIN PEER_DISCON_HANDLE_EVT */

/* USER CODE END PEER_DISCON_HANDLE_EVT */
    break;

    default:
/* USER CODE BEGIN P2PS_APP_Notification_default */

/* USER CODE END P2PS_APP_Notification_default */
      break;
  }
/* USER CODE BEGIN P2PS_APP_Notification_2 */

/* USER CODE END P2PS_APP_Notification_2 */
  return;
}

void P2PS_APP_Init(void)
{
/* USER CODE BEGIN P2PS_APP_Init */
	/* Register task used to update the characteristic (send the notification) */
		//UTIL_SEQ_RegTask(1 << CFG_MY_TASK_NOTIFY_DATA ,UTIL_SEQ_RFU, P2PS_Send_Notification_Task);
		UTIL_SEQ_RegTask(1 << SEND_FSR_DATA_BLE_TASK, UTIL_SEQ_RFU, P2PS_Send_Notification_Task_FSR);
		UTIL_SEQ_RegTask(1 << SEND_ACC_DATA_BLE_TASK, UTIL_SEQ_RFU, P2PS_Send_Notification_Task_ACC);
		UTIL_SEQ_RegTask(1 << SEND_PPG_DATA_BLE_TASK, UTIL_SEQ_RFU, P2PS_Send_Notification_Task_PPG);

	  /* Create timer to change the Temperature and update charecteristic */
//	  HW_TS_Create(CFG_TIM_PROC_ID_ISR,
//		        &(P2P_Server_App_Context.Update_timer_Id),
//		        hw_ts_Repeated,
//				P2PS_DataChange_Timer_Callback);

	  /**
	   * Initialize Template application context
	   */
	  P2P_Server_App_Context.NotificationStatus=0;
	  P2PS_APP_context_Init();
/* USER CODE END P2PS_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

static void P2PS_Send_Notification_Task_FSR(void)
{
  /* Update P2P_NOTIFY characteristic */
	uint8_t value[PAYLOAD_LENGTH] = {0};

	if(P2P_Server_App_Context.NotificationStatus)
	{
		prepare_fsr_ble_pack(&value[0]);
//		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
		P2PS_STM_App_Update_Char(P2P_NOTIFY_CHAR_UUID,&value[0]);
	}
	else
	{

	}

  return;
}
static void P2PS_Send_Notification_Task_ACC(void)
{
  /* Update P2P_NOTIFY characteristic */
	uint8_t value[PAYLOAD_LENGTH] = {0};

	if(P2P_Server_App_Context.NotificationStatus)
	{
		prepare_acc_ble_pack(&value[0]);
//		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
		P2PS_STM_App_Update_Char(P2P_NOTIFY_CHAR_UUID,&value[0]);
	}
	else
	{

	}

  return;
}
static void P2PS_Send_Notification_Task_PPG(void)
{
  /* Update P2P_NOTIFY characteristic */
	uint8_t value[PAYLOAD_LENGTH] = {0};

	if(P2P_Server_App_Context.NotificationStatus)
	{
		prepare_ppg_ble_pack(&value[0]);
//		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
		P2PS_STM_App_Update_Char(P2P_NOTIFY_CHAR_UUID,&value[0]);
	}
	else
	{

	}

  return;
}

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
static void P2PS_APP_context_Init(void)
{
  P2P_Server_App_Context.Parameter = 0;
  P2P_Server_App_Context.ACC.X = 0;
  P2P_Server_App_Context.ACC.Y = 0;
  P2P_Server_App_Context.ACC.Z = 0;
  P2P_Server_App_Context.ECG.ecg_value = 0;
  P2P_Server_App_Context.PPG.ppg_value = 0;
}

static void P2PS_DataChange_Timer_Callback(void)
{
	UTIL_SEQ_SetTask(1 << CFG_MY_TASK_NOTIFY_DATA, CFG_SCH_PRIO_0);
}
/* USER CODE END FD_LOCAL_FUNCTIONS*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
