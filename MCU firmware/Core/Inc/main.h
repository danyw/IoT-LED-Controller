/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32wbxx_hal.h"

#include "app_conf.h"
#include "app_entry.h"
#include "app_common.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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
#define ADC_VOLTAGE_Pin GPIO_PIN_0
#define ADC_VOLTAGE_GPIO_Port GPIOC
#define ADC_CURRENT_Pin GPIO_PIN_1
#define ADC_CURRENT_GPIO_Port GPIOC
#define ADC_TEMP_Pin GPIO_PIN_2
#define ADC_TEMP_GPIO_Port GPIOC
#define TIM1_CH1_Pin GPIO_PIN_8
#define TIM1_CH1_GPIO_Port GPIOA
#define B1_Pin GPIO_PIN_4
#define B1_GPIO_Port GPIOC
#define LD2_Pin GPIO_PIN_0
#define LD2_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_1
#define LD3_GPIO_Port GPIOB
#define soft_en_24v_Pin GPIO_PIN_13
#define soft_en_24v_GPIO_Port GPIOB
#define JTMS_Pin GPIO_PIN_13
#define JTMS_GPIO_Port GPIOA
#define JTCK_Pin GPIO_PIN_14
#define JTCK_GPIO_Port GPIOA
#define led_enable_Pin GPIO_PIN_10
#define led_enable_GPIO_Port GPIOC
#define B2_Pin GPIO_PIN_0
#define B2_GPIO_Port GPIOD
#define B3_Pin GPIO_PIN_1
#define B3_GPIO_Port GPIOD
#define JTDO_Pin GPIO_PIN_3
#define JTDO_GPIO_Port GPIOB
#define LD1_Pin GPIO_PIN_5
#define LD1_GPIO_Port GPIOB
#define STLINK_RX_Pin GPIO_PIN_6
#define STLINK_RX_GPIO_Port GPIOB
#define STLINK_TX_Pin GPIO_PIN_7
#define STLINK_TX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define PWM_CHANNELS_MAX 6

extern uint16_t rawValues[3]; // Assuming 3 ADC channels: Voltage, Current, Temperature

typedef struct {
	uint8_t  PWM_CHANNELS;
	bool Pwm_on[6];
	uint16_t pwmx_brightness[PWM_CHANNELS_MAX][24];
    float CurrentSenseFactor;
    float TemperatureOffset;
} SystemSettings_t;

extern SystemSettings_t SystemSettings;



/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
