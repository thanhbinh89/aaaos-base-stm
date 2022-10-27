/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
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
#define INA_S2_Pin GPIO_PIN_2
#define INA_S2_GPIO_Port GPIOE
#define INA_S1_Pin GPIO_PIN_3
#define INA_S1_GPIO_Port GPIOE
#define INA_S0_Pin GPIO_PIN_4
#define INA_S0_GPIO_Port GPIOE
#define LED_LIFE_Pin GPIO_PIN_5
#define LED_LIFE_GPIO_Port GPIOE
#define I2C2_SDA_Pin GPIO_PIN_0
#define I2C2_SDA_GPIO_Port GPIOF
#define I2C2_SCL_Pin GPIO_PIN_1
#define I2C2_SCL_GPIO_Port GPIOF
#define INA9_Pin GPIO_PIN_4
#define INA9_GPIO_Port GPIOF
#define INA10_Pin GPIO_PIN_5
#define INA10_GPIO_Port GPIOF
#define ADC_INA_Pin GPIO_PIN_6
#define ADC_INA_GPIO_Port GPIOF
#define ADC_INB_Pin GPIO_PIN_7
#define ADC_INB_GPIO_Port GPIOF
#define ADC_OVD_Pin GPIO_PIN_8
#define ADC_OVD_GPIO_Port GPIOF
#define ADC_VIN_Pin GPIO_PIN_9
#define ADC_VIN_GPIO_Port GPIOF
#define ADC_CT1_Pin GPIO_PIN_3
#define ADC_CT1_GPIO_Port GPIOA
#define ADC_CT2_Pin GPIO_PIN_4
#define ADC_CT2_GPIO_Port GPIOA
#define ADC_CT3_Pin GPIO_PIN_5
#define ADC_CT3_GPIO_Port GPIOA
#define ADC_CT4_Pin GPIO_PIN_6
#define ADC_CT4_GPIO_Port GPIOA
#define ADC_NTC1_Pin GPIO_PIN_0
#define ADC_NTC1_GPIO_Port GPIOB
#define ADC_NTC2_Pin GPIO_PIN_1
#define ADC_NTC2_GPIO_Port GPIOB
#define SH_LD_Pin GPIO_PIN_0
#define SH_LD_GPIO_Port GPIOG
#define CLK_Pin GPIO_PIN_7
#define CLK_GPIO_Port GPIOE
#define BUZZER_Pin GPIO_PIN_8
#define BUZZER_GPIO_Port GPIOE
#define LED_SST_Pin GPIO_PIN_9
#define LED_SST_GPIO_Port GPIOE
#define LCD_RS_Pin GPIO_PIN_10
#define LCD_RS_GPIO_Port GPIOE
#define LCD_RST_Pin GPIO_PIN_11
#define LCD_RST_GPIO_Port GPIOE
#define LCD_BL_Pin GPIO_PIN_12
#define LCD_BL_GPIO_Port GPIOE
#define SER_IN_Pin GPIO_PIN_13
#define SER_IN_GPIO_Port GPIOE
#define SRCLK_Pin GPIO_PIN_14
#define SRCLK_GPIO_Port GPIOE
#define RCLK_Pin GPIO_PIN_15
#define RCLK_GPIO_Port GPIOE
#define DEBUG_TX_Pin GPIO_PIN_10
#define DEBUG_TX_GPIO_Port GPIOB
#define DEBUG_RX_Pin GPIO_PIN_11
#define DEBUG_RX_GPIO_Port GPIOB
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define RL_LATCH_Pin GPIO_PIN_9
#define RL_LATCH_GPIO_Port GPIOD
#define RL_OUT8_Pin GPIO_PIN_10
#define RL_OUT8_GPIO_Port GPIOD
#define RL_OUT7_Pin GPIO_PIN_11
#define RL_OUT7_GPIO_Port GPIOD
#define RL_OUT6_Pin GPIO_PIN_12
#define RL_OUT6_GPIO_Port GPIOD
#define RL_OUT5_Pin GPIO_PIN_13
#define RL_OUT5_GPIO_Port GPIOD
#define ETH_CR_EN_Pin GPIO_PIN_14
#define ETH_CR_EN_GPIO_Port GPIOD
#define RL_OUT4_Pin GPIO_PIN_2
#define RL_OUT4_GPIO_Port GPIOG
#define RL_OUT3_Pin GPIO_PIN_3
#define RL_OUT3_GPIO_Port GPIOG
#define RL_OUT2_Pin GPIO_PIN_4
#define RL_OUT2_GPIO_Port GPIOG
#define RL_OUT1_Pin GPIO_PIN_5
#define RL_OUT1_GPIO_Port GPIOG
#define ETH_NRST_Pin GPIO_PIN_6
#define ETH_NRST_GPIO_Port GPIOG
#define BOX_PG7_Pin GPIO_PIN_7
#define BOX_PG7_GPIO_Port GPIOG
#define BOX_PG8_Pin GPIO_PIN_8
#define BOX_PG8_GPIO_Port GPIOG
#define SMK_OUT6_Pin GPIO_PIN_8
#define SMK_OUT6_GPIO_Port GPIOC
#define VBR_OUT5_Pin GPIO_PIN_8
#define VBR_OUT5_GPIO_Port GPIOA
#define SIN_OUT4_Pin GPIO_PIN_9
#define SIN_OUT4_GPIO_Port GPIOA
#define SIN_OUT3_Pin GPIO_PIN_10
#define SIN_OUT3_GPIO_Port GPIOA
#define SIN_OUT2_Pin GPIO_PIN_11
#define SIN_OUT2_GPIO_Port GPIOA
#define SIN_OUT1_Pin GPIO_PIN_12
#define SIN_OUT1_GPIO_Port GPIOA
#define LCD_EN_Pin GPIO_PIN_10
#define LCD_EN_GPIO_Port GPIOC
#define SER_OUT_Pin GPIO_PIN_11
#define SER_OUT_GPIO_Port GPIOC
#define LCD_RW_Pin GPIO_PIN_12
#define LCD_RW_GPIO_Port GPIOC
#define INB_S0_Pin GPIO_PIN_0
#define INB_S0_GPIO_Port GPIOD
#define INB_S1_Pin GPIO_PIN_1
#define INB_S1_GPIO_Port GPIOD
#define INB_S2_Pin GPIO_PIN_2
#define INB_S2_GPIO_Port GPIOD
#define RL_CMR1_Pin GPIO_PIN_3
#define RL_CMR1_GPIO_Port GPIOD
#define RL_CMR2_Pin GPIO_PIN_4
#define RL_CMR2_GPIO_Port GPIOD
#define ETH_PWR_Pin GPIO_PIN_7
#define ETH_PWR_GPIO_Port GPIOD
#define RS485_PWR_Pin GPIO_PIN_4
#define RS485_PWR_GPIO_Port GPIOB
#define RS485_DIR_Pin GPIO_PIN_5
#define RS485_DIR_GPIO_Port GPIOB
#define RS485_TX_Pin GPIO_PIN_6
#define RS485_TX_GPIO_Port GPIOB
#define RS485_RX_Pin GPIO_PIN_7
#define RS485_RX_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define SNTP_SET_SYSTEM_TIME(sec) setTimestamp(sec)
#define SNTP_GET_SYSTEM_TIME(sec, us) getTimestamp(&sec)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
