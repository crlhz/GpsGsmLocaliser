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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "gps.h"
#include "gsm.h"
#include "string.h"
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
RTC_TimeTypeDef cTime;
RTC_DateTypeDef cDate;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
char message[180];
char doubletoLCD[10];
uint16_t size;
uint8_t CompareSeconds=0;
uint8_t protect=0;
double compareLatitude=0;
double compareLongitude=0;
uint8_t test=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void set_time();
void set_date();

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
  uint8_t timeToResponse=0;
  uint8_t mode1Minutes=0;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  HAL_Delay(3000);							//wait for GSM module
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  LCD_PortType Ports[] = {D4_GPIO_Port, D5_GPIO_Port, D6_GPIO_Port, D7_GPIO_Port};
  LCD_PinType Pins[] = {D4_Pin, D5_Pin, D6_Pin, D7_Pin};
  LCD_HandleTypeDef LCD;
  LCD = LCD_Create(Ports, Pins, RS_GPIO_Port, RS_Pin, EN_GPIO_Port, EN_Pin);

  LCD_Clear(&LCD);

  GSM_Init(&huart2);


  size=sprintf(message, "AT+CMGF=1\r");					//switch on text mode
  HAL_UART_Transmit(&huart2, message, size, 50);
  HAL_Delay(50);
  size=sprintf(message, "AT+CMGD=1,4\r");				//delete all SMS
  HAL_UART_Transmit(&huart2, message, size, 50);
  HAL_Delay(50);

  LCD_Clear(&LCD);


  //**Boot menu - wait till get number**//
  controlLCD=0;
  while(strlen(number)!=9){
	  GSM_Work();
	  LCD_Cursor(&LCD,0,0);
	  LCD_String(&LCD, "***loCARliser*");
	  LCD_Cursor(&LCD,0,15);
	  LCD_Int(&LCD,test);
	  LCD_Cursor(&LCD,1,0);
	  LCD_String(&LCD, "  Brak  numeru  ");
  }

  MX_USART1_UART_Init();
  GPS_Init(&huart1);
  LCD_Clear(&LCD);

  //**Sequence for welcome message**//
  GSM_Response=1;
  GSM_SelectNumber();
  utc=1;
  mode=10;
  interval=60;											//default interval between messages in Mode 1
  timeSetFlag=0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  	  HAL_RTC_GetTime(&hrtc, &cTime, RTC_FORMAT_BIN);
	  	  HAL_RTC_GetDate(&hrtc, &cDate, RTC_FORMAT_BIN);
	  	  GPS_Work();
	  	  GSM_Work();

	  	  if(cTime.Seconds != CompareSeconds){
	  		    //**LCD Display section**//
	  		    if(controlLCD<10){
	  		    	LCD_Cursor(&LCD, 0, 0);
	  		    	LCD_Int(&LCD, (cTime.Hours));
	  		    	LCD_Cursor(&LCD, 0, 2);
	  		    	LCD_String(&LCD, ":");
	  		    	LCD_Cursor(&LCD, 0, 3);
	  		    	LCD_Int(&LCD, cTime.Minutes);
	  		    	LCD_Cursor(&LCD, 0, 5);
	  		    	LCD_String(&LCD, ":");
	  		    	LCD_Cursor(&LCD, 0, 6);
	  		    	LCD_Int(&LCD, cTime.Seconds);
	  		    	LCD_Cursor(&LCD, 1, 0);
	  		    	LCD_Int(&LCD, cDate.Date);
	  		    	LCD_Cursor(&LCD, 1, 2);
	  		    	LCD_String(&LCD, ".");
	  		    	LCD_Cursor(&LCD, 1, 3);
	  		    	LCD_Int(&LCD, cDate.Month);
	  		    	LCD_Cursor(&LCD, 1, 5);
	  		    	LCD_String(&LCD, ".");
	  		    	LCD_Cursor(&LCD, 1, 6);
	  		    	LCD_Int(&LCD, (cDate.Year+2000));
	  		    	}

	  		    if(controlLCD==10){
	  		    	LCD_Clear(&LCD);
	  		    }

	  		    if(controlLCD>=10){
	  		    	if(GPS_Valid=='A'){
	  		    		LCD_Cursor(&LCD,0,0);
	  		    		sprintf(doubletoLCD, "%6f%c",GPS_Latitude, GPS_LatitudeDirection);
	  		    		LCD_String(&LCD,doubletoLCD);
	  		    		LCD_Cursor(&LCD,1,0);
	  		    		sprintf(doubletoLCD, "%6f%c",GPS_Longitude,GPS_LongitudeDirection);
	  		    		LCD_String(&LCD,doubletoLCD);
	  		    	}else{
	  		    		LCD_Cursor(&LCD, 0,0);
	  		    		LCD_String(&LCD,"Brak sygnalu GPS");
	  		    		LCD_Cursor(&LCD, 1,0);
	  		    		LCD_String(&LCD,"****************");
	  		    	}
	  		    }

	  		    if(controlLCD==20){
	  		    	LCD_Clear(&LCD);
	  		    	controlLCD=1;
	  		    }

	  		  //**Mode 1 section**//
	  		  if(mode==1){
	  			 if(compareMinutes!=cTime.Minutes){
	  				 mode1Minutes++;
	  				 compareMinutes=cTime.Minutes;
	  			 }
	  			if(mode1Minutes==interval){
	  				mode1Minutes=0;
	  				GSM_SelectNumber();
	  				GSM_Response=3;
	  			}
	  		  }

	  		  //**Mode 2 section**//
	  		  if((mode==2)&&(protect==1)){
	  			  if((abs(GPS_Latitude-compareLatitude)>0.001)||(abs(GPS_Longitude-compareLongitude)>0.001)){
	  				  GSM_SelectNumber();
	  				  GSM_Response=4;
	  			  }
	  		  }

	  		 //**SMS sending section**//
	  		  switch(GSM_Response){
	  		  case 1:
	  		  		  timeToResponse++;
	  		  		  if(timeToResponse==2){
	  		  			GSM_SendMessage("Witaj!\nWybierz tryb:\nMODE 0 - Tryb czuwania\nMODE 1 - Tryb podstawowy\nMODE 2- Tryb ochrony");
	  		  			GSM_Response=0;
	  		  			timeToResponse=0;
	  		  		  }
	  		  		  break;

	  		  case 2:
	  		  		  timeToResponse++;
	  		  		  if((timeToResponse==2)&&mode==0){
	  		  			  GSM_SendMessage("Wybrano tryb czuwania");
	  		  			  protect=0;
	  		  			  GSM_Response=0;
	  		  			  timeToResponse=0;
	  		  		  }
	  		  		  if((timeToResponse==2)&&mode==1){
	  		  			  GSM_SendMessage("Wybrano tryb podstawowy");
	  		  			  protect=0;
	  		  			  GSM_Response=0;
	  		  			  timeToResponse=0;
	  		  		  }
	  		  		  if((timeToResponse==2)&&mode==2){
	  		  			  GSM_SendMessage("Wybrano tryb ochrony");
	  		  			  protect=1;
	  		  			  GSM_Response=0;
	  		  			  timeToResponse=0;
	  		  			  compareLatitude=GPS_Latitude;
	  		  			  compareLongitude=GPS_Longitude;
	  		  		  }
	  		  		  break;

	  		  case 3:
	  		  		  timeToResponse++;
	  		  		  if(timeToResponse==2){
	  		  			  if(GPS_Valid=='A'){
	  		  				GSM_LocateMessage();
	  		  				GSM_Response=0;
	  		  				timeToResponse=0;
	  		  			  }else{
	  		  				GSM_SendMessage("Brak sygnalu GPS");
	  		  				GSM_Response=0;
	  		  				timeToResponse=0;
	  		  			  }
	  		  		  }
	  			  	  break;

	  		  case 4:
	  			  	  if(timeToResponse==2){
	  			  		  GSM_SendMessage("KRADZIEZ!");
	  			  		  GSM_Response=0;
	  			  		  timeToResponse=0;
	  			  	  }
	  			  	  break;
	  		  }

	  		 controlLCD++;
	  		CompareSeconds=cTime.Seconds;
	  	  }

	  	  //**Time synchronization**//
	  	  if((cTime.Hours==1)&&(cTime.Minutes==0)&&(cTime.Seconds==0)){
	  		  timeSetFlag=0;
	  	  }

	  	  //**Setting time with UTC correction**//
	  	  if(GPS_Valid=='A'&&timeSetFlag==0){
	  		  if(GPS_Hour>23){
	  			  GPS_Hour=GPS_Hour-24;
	  			  GPS_Day++;
	  		  }
	  		  set_time();
	  		  set_date();
	  		  timeSetFlag=1;
	  	  }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 1;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 1;
  DateToUpdate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|D6_Pin|D5_Pin|D4_Pin
                          |RS_Pin|EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 D6_Pin D5_Pin D4_Pin
                           RS_Pin EN_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|D6_Pin|D5_Pin|D4_Pin
                          |RS_Pin|EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : D7_Pin */
  GPIO_InitStruct.Pin = D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(D7_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart1){
	if(huart1->Instance == USART1){
		GPS_UartReceiveChar();
	}

	if(huart1->Instance == USART2){
		GSM_UartReceiveChar();
		test=1;
	}
}

void set_time(){
	  RTC_TimeTypeDef sTime;
	  sTime.Hours = GPS_Hour;
	  sTime.Minutes = GPS_Minute;
	  sTime.Seconds = GPS_Second;

	  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void set_date(){
	  RTC_DateTypeDef DateToUpdate;
	  DateToUpdate.Month = GPS_Month;
	  DateToUpdate.Date = GPS_Day;
	  DateToUpdate.Year = GPS_Year;

	  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
	  {
	    Error_Handler();
	  }
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
