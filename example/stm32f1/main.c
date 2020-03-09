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
#include <stdio.h>
#include "uc_chrono.h"         // https://github.com/mnemocron/libDaylength
#include "lib_daylength.h"     // https://github.com/mnemocron/libDaylength
#include "ssd1306.h"           // https://github.com/mnemocron/stm32-ssd1306
#include "compile_time.h"      // https://stackoverflow.com/questions/11697820/how-to-use-date-and-time-predefined-macros-in-as-two-integers-then-stri

/**
  @brief This code demonstrates the calculation of sunrise and sunset time on 
          an STM32F1 microcontroller.
  @details An SSD1306 OLED display is connected to I2C interface
           Two LEDs (blue/yellow) or (cold white/ warm white) are connected to 
           PWM GPIO Pins
           During the Day ((t_sunrise + 1h)  <  t  < (t_sunset - 1h)),
           the blue LED lights up. After sunrise and before sunset, 
           the blue LED fades, as the yellow starts to light up.
           During nighttime, the yellow LED is on.
 */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t rxBuffer;
SSD1306_t holed;

const uint32_t compiled_year = BUILD_YEAR;
const uint8_t compiled_month = BUILD_MONTH;
const uint8_t compiled_day = BUILD_DAY;
const uint8_t compiled_hour = BUILD_HOUR;
const uint8_t compiled_min = BUILD_MIN;
const uint8_t compiled_sec = BUILD_SEC;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void setPWM(TIM_HandleTypeDef, uint32_t, uint16_t, uint16_t);
void setLEDs(uint16_t);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* PRINTF REDIRECT to UART BEGIN */
// @see    http://www.keil.com/forum/60531/
// @see    https://stackoverflow.com/questions/45535126/stm32-printf-redirect

struct __FILE{
  int handle;
  /* Whatever you require here. If the only file you are using is */
  /* standard output using printf() for debugging, no file handling */
  /* is required. */
};

FILE __stdout;

int fputc(int ch, FILE *f){
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

int ferror(FILE *f){
  /* Your implementation of ferror(). */
  return 0;
}
/* PRINTF REDIRECT to UART END */

void RTC_Set_Compiled_Date(){
  RTC_DateTypeDef sDate;
  sDate.Year = compiled_year - 2000U;
  sDate.Month = compiled_month;
  sDate.Date = compiled_day;
  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
  printf("%d-%02d-%02d\n", sDate.Year, sDate.Month, sDate.Date);
}

void RTC_Set_Compiled_Time(){
  RTC_TimeTypeDef sTime;
  sTime.Hours = compiled_hour;
  sTime.Minutes = compiled_min;
  sTime.Seconds = compiled_sec;
  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}

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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart2, &rxBuffer, 1);
  
  holed.hi2cx = &hi2c1;
  ssd1306_Init(&holed);
  ssd1306_Fill(&holed, Black);
  ssd1306_SetCursor(&holed, 2, 2);
  ssd1306_WriteString(&holed, "RTC", Font_11x18, White);
  ssd1306_UpdateScreen(&holed);
  HAL_Delay(500);
  
  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;
  char lcd_buf[18];
  
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
  
  /**
  @bug : the STM32F1 Series is not able to store the date in the RTC (or at least it is not implemented in HAL)
  */
  // check if RTC lost the date
  if(sDate.Year <= (compiled_year-2000U)){
    if(sDate.Month <= compiled_month){
      if(sDate.Date < compiled_day){
        RTC_Set_Compiled_Date();
        printf("Date was outdated\n");
        ssd1306_SetCursor(&holed, 2, 2);
        ssd1306_WriteString(&holed, "outdated", Font_11x18, White);
        sprintf(lcd_buf, "%d-%02d-%02d", sDate.Year, sDate.Month, sDate.Date);
        ssd1306_SetCursor(&holed, 2, 20);
        ssd1306_WriteString(&holed, lcd_buf, Font_7x10, White);
        ssd1306_UpdateScreen(&holed);
        HAL_Delay(1500);
      }
    }
  }
  ssd1306_Fill(&holed, Black);
  ssd1306_UpdateScreen(&holed);
  
  // check if the RTC lost the time
  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  uint32_t s_day = sTime.Hours*3600 + sTime.Minutes*60 + sTime.Seconds;
  if(s_day < (compiled_hour*3600 + compiled_min*60 + compiled_sec)){
    RTC_Set_Compiled_Time();
    printf("Time was outdated\n");
    ssd1306_SetCursor(&holed, 2, 2);
    ssd1306_WriteString(&holed, "outtimed", Font_11x18, White);
    sprintf(lcd_buf, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Minutes);
    ssd1306_SetCursor(&holed, 2, 20);
    ssd1306_WriteString(&holed, lcd_buf, Font_7x10, White);
    sprintf(lcd_buf, "%02d:%02d:%02d", compiled_hour, compiled_min, compiled_sec);
    ssd1306_SetCursor(&holed, 2, 35);
    ssd1306_WriteString(&holed, lcd_buf, Font_7x10, White);
    ssd1306_UpdateScreen(&holed);
    HAL_Delay(1500);
  }
  ssd1306_Fill(&holed, Black);
  ssd1306_UpdateScreen(&holed);
  
  UNIXTimestamp_t st;
  st.year = sDate.Year+2000;
  st.month = sDate.Month;
  st.day = sDate.Date;
  st.hour = sTime.Hours;
  st.minute = sTime.Minutes;
  st.second = sTime.Seconds;
  
  UNIXTimestamp_t sR;
  // Latitude / Longitude for Solothurn - CH4500
  double lon = 47.207447;
  double lat = 7.537179;
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    printf("RTC: %02d:%02d:%02d\n", sTime.Hours, sTime.Minutes, sTime.Seconds);
    
    st.year = sDate.Year+2000;
    st.month = sDate.Month;
    st.day = sDate.Date;
    st.hour = sTime.Hours;
    st.minute = sTime.Minutes;
    st.second = sTime.Seconds;
    
    // current date and time
    sprintf(lcd_buf, "%d-%02d-%02d", st.year, st.month, st.day);
    ssd1306_SetCursor(&holed, 2, 2);
    ssd1306_WriteString(&holed, lcd_buf, Font_7x10, White);
    sprintf(lcd_buf, "%02d:%02d:%02d", st.hour, st.minute, st.second);
    ssd1306_SetCursor(&holed, 2, 12);
    ssd1306_WriteString(&holed, lcd_buf, Font_7x10, White);
    ssd1306_UpdateScreen(&holed);
    printf("UNIX: %d-%02d-%02d %02d:%02d:%02d\n", st.year, st.month, st.day, st.hour, st.minute, st.second);
    
    // calculate sunrise
    sR = getSunrise(&st, lon, lat);
    printf("Rise: %d-%02d-%02d %02d:%02d:%02d\n", sR.year, sR.month, sR.day, sR.hour, sR.minute, sR.second);
    sprintf(lcd_buf, "%02d:%02d:%02d -sunrise", sR.hour, sR.minute, sR.second);
    ssd1306_SetCursor(&holed, 2, 24);
    ssd1306_WriteString(&holed, lcd_buf, Font_7x10, White);
    ssd1306_UpdateScreen(&holed);
    uint32_t s_sunrise = sR.hour*3600 + sR.minute*60 + sR.second;
    
    // calculate sunset
    sR = getSunset(&st, lon, lat);
    printf("Set : %d-%02d-%02d %02d:%02d:%02d\n", sR.year, sR.month, sR.day, sR.hour, sR.minute, sR.second);
    sprintf(lcd_buf, "%02d:%02d:%02d -sunset", sR.hour, sR.minute, sR.second);
    ssd1306_SetCursor(&holed, 2, 36);
    ssd1306_WriteString(&holed, lcd_buf, Font_7x10, White);
    ssd1306_UpdateScreen(&holed);
    uint32_t s_sunset = sR.hour*3600 + sR.minute*60 + sR.second;
    
    HAL_Delay(500);
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    
    // get highest position of sun
    sR = getSunTransit(&st, lon, lat);
    uint32_t s_median = sR.hour*3600 + sR.minute*60 + sR.second;
    uint32_t s_now = st.hour*3600 + st.minute*60 + st.second;
    uint32_t pre_sunset = 3600;
    
    if(s_now < s_median){
      // ANTE MERIDIEM
      // check sunrise
      if(s_now < (s_sunrise + pre_sunset)){
        if(s_now > (s_sunrise)){
          // DAWN
          setLEDs(3600 - (s_now - s_sunrise));
          sprintf(lcd_buf, "%04ds since sunrise", s_now-s_sunrise);
        } else {
          // NIGHTTIME
          setPWM(htim3, TIM_CHANNEL_3, 255, 255);
          setPWM(htim3, TIM_CHANNEL_4, 255, 0);
          sprintf(lcd_buf, "AM (night)      .");
        }
      } else {
        // DAYTIME
        setPWM(htim3, TIM_CHANNEL_3, 255, 0);
        setPWM(htim3, TIM_CHANNEL_4, 255, 255);
        sprintf(lcd_buf, "AM (morning)    .");
      }
        
      
    } else {
      // POST MERIDIEM
      // check sunset
      if(s_now > (s_sunset - pre_sunset)){
        if(s_now > s_sunset){
          // NIGHTTIME
          setPWM(htim3, TIM_CHANNEL_3, 255, 255);
          setPWM(htim3, TIM_CHANNEL_4, 255, 0);
          sprintf(lcd_buf, "PM (night)      .");
        } else {
          // DUSK
          setLEDs(s_sunset - s_now);
          sprintf(lcd_buf, "%04ds till sunset", s_sunset-s_now);
        }
      } else {
        // DAYTIME
        setPWM(htim3, TIM_CHANNEL_3, 255, 0);
        setPWM(htim3, TIM_CHANNEL_4, 255, 255);
        sprintf(lcd_buf, "PM (afternoon)  .");
      }
    }
    ssd1306_SetCursor(&holed, 2, 48);
    ssd1306_WriteString(&holed, lcd_buf, Font_7x10, White);
    ssd1306_UpdateScreen(&holed);
    
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  /*
  sTime.Hours = 11;
  sTime.Minutes = 35;
  sTime.Seconds = 23;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_SUNDAY;
  DateToUpdate.Month = RTC_MONTH_MARCH;
  DateToUpdate.Date = 8;
  DateToUpdate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  */
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 255;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

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
  huart2.Init.BaudRate = 115200;
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
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

void setPWM(TIM_HandleTypeDef timer, uint32_t channel, uint16_t period, uint16_t pulse)
{
  HAL_TIM_PWM_Stop(&timer, channel);   
  // stop generation of pwm  
  TIM_OC_InitTypeDef sConfigOC;   
  timer.Init.Period = period;           
  // set the period duration  
  HAL_TIM_PWM_Init(&timer);  
  // reinititialise with new period value 
  sConfigOC.OCMode = TIM_OCMODE_PWM1;   
  sConfigOC.Pulse = pulse;              
  // set the pulse duration   
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;   
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;   
  HAL_TIM_PWM_ConfigChannel(&timer, &sConfigOC, channel);   
  HAL_TIM_PWM_Start(&timer, channel);   // start pwm generation} 
}

void setLEDs(uint16_t t)
{
  // pulse --> yellow
  uint16_t pulse = t / 14;
  if(pulse > 255)
    pulse = 255;
  setPWM(htim3, TIM_CHANNEL_3, 255, pulse);  // CH3 = yellow
  setPWM(htim3, TIM_CHANNEL_4, 255, 255-pulse); // CH4 = blue
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
