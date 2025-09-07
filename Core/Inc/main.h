/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define RX_BUFFER_SIZE 10

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_PIN_Pin GPIO_PIN_13
#define LED_PIN_GPIO_Port GPIOC
#define ENCODER_AIN1_Pin GPIO_PIN_3
#define ENCODER_AIN1_GPIO_Port GPIOA
#define ENCODER_AIN2_Pin GPIO_PIN_4
#define ENCODER_AIN2_GPIO_Port GPIOA
#define BUTTON_REST_Pin GPIO_PIN_0
#define BUTTON_REST_GPIO_Port GPIOB
#define BUTTON_SET_Pin GPIO_PIN_1
#define BUTTON_SET_GPIO_Port GPIOB
#define BUTTON_M_Pin GPIO_PIN_3
#define BUTTON_M_GPIO_Port GPIOB
#define BUTTON_R_Pin GPIO_PIN_4
#define BUTTON_R_GPIO_Port GPIOB
#define BUTTON_L_Pin GPIO_PIN_5
#define BUTTON_L_GPIO_Port GPIOB
#define BUTTON_D_Pin GPIO_PIN_6
#define BUTTON_D_GPIO_Port GPIOB
#define BUTTON_U_Pin GPIO_PIN_7
#define BUTTON_U_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
