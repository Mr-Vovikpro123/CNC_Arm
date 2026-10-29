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
typedef enum {
	// for writing and reading
	W_OK     = 0,
	SD_Err   = 1,
	File_Err = 2,
	// for machine control
	MC_OK	 = 0, 
	Mem_Err  = 3,
	Angle_Err = 360,
	
	Pen_Up = 1,
	Pen_Down = 0
} WRESULT;

	
typedef enum {
	BUFheight = 50,
	BUFwidth  = 50
} BUFquality;
	
typedef enum {
	WHITE = 255,
	BLACK = 0
} Colours;
	
typedef enum {
	arm_length       = 200,
	CMtoSheet_height = 30,
	CMtoSheet_width  = 30,
	A4               = 4,
	A4h              = 297,
	A4w              = 210,
	A4album          = 40,
	A3               = 3,
	A3h              = 420,
	A3w              = 297,
	A3album          = 30
} SIZESmm;
	
typedef struct { unsigned int x, y; }							Coords;
typedef struct {double x, y;}									PaperCoords; 	
typedef struct { unsigned int height, width; }					Quality;
typedef struct { double CM, SM; }								M_Angles;
typedef struct { double min_x, min_y, max_x, max_y; }			Square;
// typedef struct { GPIO_TypeDef* GPIO_Port; uint16_t GPIO_Pin; }	GPIO_PinConfig;
typedef struct {
	GPIO_TypeDef* CM_DIR_GPIO_Port; 
	uint16_t CM_DIR_Pin;
	GPIO_TypeDef* CM_PUL_GPIO_Port; 
	uint16_t CM_PUL_Pin;
	GPIO_TypeDef* SM_DIR_GPIO_Port; 
	uint16_t SM_DIR_Pin;
	GPIO_TypeDef* SM_PUL_GPIO_Port; 
	uint16_t SM_PUL_Pin;

} MotorsPinConfig;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SD_CS_Pin GPIO_PIN_12
#define SD_CS_GPIO_Port GPIOB
#define SM_DIR_Pin GPIO_PIN_9
#define SM_DIR_GPIO_Port GPIOA
#define SM_ENA_Pin GPIO_PIN_10
#define SM_ENA_GPIO_Port GPIOA
#define CM_PUL_Pin GPIO_PIN_11
#define CM_PUL_GPIO_Port GPIOA
#define CM_DIR_Pin GPIO_PIN_12
#define CM_DIR_GPIO_Port GPIOA
#define CM_ENA_Pin GPIO_PIN_15
#define CM_ENA_GPIO_Port GPIOA
#define SM_PUL_Pin GPIO_PIN_8
#define SM_PUL_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
