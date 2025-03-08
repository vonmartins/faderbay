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
#include "stm32f4xx_hal.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "C:\Development\Repos\faderbay\Apps\cli\cli.h"
#include "flash.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern cli_t cli; 
extern Flash_Write_Block_t device_config_block;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MUX1_S0_Pin GPIO_PIN_7
#define MUX1_S0_GPIO_Port GPIOA
#define MUX1_S1_Pin GPIO_PIN_4
#define MUX1_S1_GPIO_Port GPIOC
#define MUX1_S2_Pin GPIO_PIN_5
#define MUX1_S2_GPIO_Port GPIOC
#define MUX2_S0_Pin GPIO_PIN_0
#define MUX2_S0_GPIO_Port GPIOB
#define MUX2_S1_Pin GPIO_PIN_1
#define MUX2_S1_GPIO_Port GPIOB
#define MUX2_S2_Pin GPIO_PIN_2
#define MUX2_S2_GPIO_Port GPIOB
#define BTTN1_Pin GPIO_PIN_7
#define BTTN1_GPIO_Port GPIOE
#define BTTN2_Pin GPIO_PIN_8
#define BTTN2_GPIO_Port GPIOE
#define BTTN3_Pin GPIO_PIN_9
#define BTTN3_GPIO_Port GPIOE
#define BTTN4_Pin GPIO_PIN_10
#define BTTN4_GPIO_Port GPIOE
#define MUX1_INH_Pin GPIO_PIN_11
#define MUX1_INH_GPIO_Port GPIOE
#define MUX2_INH_Pin GPIO_PIN_12
#define MUX2_INH_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
