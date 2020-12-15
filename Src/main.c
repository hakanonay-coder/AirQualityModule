/* AIR QUALITY MEASURMENT MODULE FOR STMF103 */
/**
  ******************************************************************************
  * @file           : AirQualitySensorModule.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  *	Untested code of the project "AIR QUALITY MEASURMENT MODULE" for Novumare Technologies. HAL Drivers
  *	and Standart Libraries are used.
  *
  *		by : Hakan_Onay
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "math.h"
#include "string.h"

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
UART_HandleTypeDef huart2;



/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC2_Init(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{




  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();



  /* Configure the system clock */
  SystemClock_Config();



  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_ADC2_Init();


  uint8_t start[]="\r\n-----------Hava Ölçüm Modülü Çalışıyor-------\r\n";
  HAL_UART_Transmit_DMA(&huart2, start, sizeof(start) - 1);
  HAL_Delay(1000);

  while (1)
  {
    /* USER CODE END WHILE */

	  uint16_t rawvalue1 = 0;
	  uint16_t rawvalue2 = 0;
	  // Start ADC1 Conversion
	  HAL_ADC_Start(&hadc1);
	  // Poll ADC1 Perihperal & TimeOut = 1mSec
	  HAL_ADC_PollForConversion(&hadc1, 1);
	  rawvalue1 = HAL_ADC_GetValue(&hadc1);
	  HAL_Delay(3);
	  // Start ADC2 Conversion
	  HAL_ADC_Start(&hadc2);
	  // Poll ADC2 Perihperal & TimeOut = 1mSec
	  HAL_ADC_PollForConversion(&hadc2, 1);
	  rawvalue2 = HAL_ADC_GetValue(&hadc2);
	  HAL_Delay(3);

	  //Quality measurement for Sensor 1
	  float temp1=(float)rawvalue1*(3.3/4096);
	  int quality1=pow((11.5428 * 35.904 * temp1 )/(25.5 - 5.1 * temp1),1.0/0.6549);

	  //Quality measurement for Sensor 2
	  float temp=(float)rawvalue2*(3.3/4096);
	  int quality2=pow((11.5428 * 35.904 * temp )/(25.5 - 5.1 * temp),1.0/0.6549);

	  if( ((75 < quality1) && (quality1 < 500) ) || ((75 < quality2) && (quality2 < 500))){
		  HAL_GPIO_DeInit(GPIOB, SLED1_Pin|SLED2_Pin|SLED1B15_Pin); // Close all LEDs
		  HAL_GPIO_Init(GPIOB,SLED1B15_Pin);
		  HAL_GPIO_WritePin(GPIOB,SLED1B15_Pin,SET); // Blue Led is on, meaning its safe, but there are few VOCs present
		   uint8_t msg1[]="\r\n GAZ MİKTARI : ORTA\r\n"; //Information being sent to user with Asynchronous communication with SIM7100A
		   HAL_UART_Transmit_DMA(&huart2, msg1, sizeof(msg1) - 1);
		   HAL_Delay(1000);

	  }
	  else if(( 500 < quality2 ) || ( 500 < quality1 )){
		  HAL_GPIO_DeInit(GPIOB, SLED1_Pin|SLED2_Pin|SLED1B15_Pin); // Close all LEDs
		  HAL_GPIO_Init(GPIOB,SLED1_Pin);
		  HAL_GPIO_WritePin(GPIOB,SLED1_Pin,SET); // Red Led is On, meaning its not safe, at least one of VOCs is at critical level
		  uint8_t msg2[]="\r\n GAZ MİKTARI : TEHLİKELİ\r\n";
		  HAL_UART_Transmit_DMA(&huart2, msg2, sizeof(msg2) - 1); //Information being sent to user with Asynchronous communication with SIM7100A
		  HAL_Delay(1000);

	  }
	  else if((quality1 < 75) && (quality2 < 75)){
			  HAL_GPIO_DeInit(GPIOB, SLED1_Pin|SLED2_Pin|SLED1B15_Pin); // Close all LEDs
			  HAL_GPIO_Init(GPIOB,SLED2_Pin);
			  HAL_GPIO_WritePin(GPIOB,SLED2_Pin,SET); // Green Led is On, meaning its safe
			  uint8_t msg3[]="\r\n GAZ MİKTARI : TEMİZ\r\n";
			  HAL_UART_Transmit_DMA(&huart2, msg3, sizeof(msg3) - 1); //Information being sent to user with Asynchronous communication with SIM7100A
			  HAL_Delay(1000);

		  }
	  else{
		  HAL_GPIO_DeInit(GPIOB, SLED1_Pin|SLED2_Pin|SLED1B15_Pin); //Close all LEDS,

		  uint8_t retry[]="\r\n HATALI ÖLÇÜM.  ÖLÇÜM TEKRAR BAŞLATILIYOR.\r\n";
		  HAL_UART_Transmit_DMA(&huart2, retry, sizeof(retry) - 1); //Information being sent to user with Asynchronous communication with SIM7100A
		  HAL_Delay(1000);
		  //Wait for another measurement


	  }

  }

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{



  ADC_ChannelConfTypeDef sConfig = {0};


  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }


}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{



  ADC_ChannelConfTypeDef sConfig = {0};


  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }


}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{


  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }


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
  HAL_GPIO_WritePin(USART2_DTR_GPIO_Port, USART2_DTR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SLED1_Pin|SLED2_Pin|SLED1B15_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED2_Pin */
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USART2_DTR_Pin */
  GPIO_InitStruct.Pin = USART2_DTR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USART2_DTR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USART2_DCD_Pin USART2_RI_Pin */
  GPIO_InitStruct.Pin = USART2_DCD_Pin|USART2_RI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SLED1_Pin SLED2_Pin SLED1B15_Pin */
  GPIO_InitStruct.Pin = SLED1_Pin|SLED2_Pin|SLED1B15_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}


/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{

  /* User can add his own implementation to report the HAL error return state */


}

