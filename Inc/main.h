/* AIR QUALTY MEASUREMENT MODULE Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  *
  *		by : Hakan_Onay
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOC
#define USART2_DTR_Pin GPIO_PIN_4
#define USART2_DTR_GPIO_Port GPIOA
#define USART2_DCD_Pin GPIO_PIN_5
#define USART2_DCD_GPIO_Port GPIOA
#define USART2_RI_Pin GPIO_PIN_6
#define USART2_RI_GPIO_Port GPIOA
#define SLED1_Pin GPIO_PIN_13
#define SLED1_GPIO_Port GPIOB
#define SLED2_Pin GPIO_PIN_14
#define SLED2_GPIO_Port GPIOB
#define SLED1B15_Pin GPIO_PIN_15
#define SLED1B15_GPIO_Port GPIOB


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/***************END OF FILE****/
