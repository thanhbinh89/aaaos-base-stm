/*
 * sys_io.c
 *
 *  Created on: Sep 26, 2022
 *      Author: binh
 */
#include "main.h"
#include "spi.h"
#include "iwdg.h"
#include "usart.h"
#include "sys_io.h"

#define GET_BIT(byte, index) ((byte >> index) & 0x01)

void setBuzzer(bool on) {
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setBuzzerToggle() {
	HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
}

void setLedLife(bool on) {
	HAL_GPIO_WritePin(LED_LIFE_GPIO_Port, LED_LIFE_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void toggleLedLife() {
	HAL_GPIO_TogglePin(LED_LIFE_GPIO_Port, LED_LIFE_Pin);
}

void setLedStatus(bool on) {
	HAL_GPIO_WritePin(LED_SST_GPIO_Port, LED_SST_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setFlashSpiCS(bool on) {
	HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

uint8_t tranferFlashSpi(uint8_t send) {
	uint8_t recv = 0xFF;
	HAL_SPI_TransmitReceive(&hspi2, &send, &recv, 1, 100);
	return recv;
}

const char* getRstReason(bool clear) {
	char *res;
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) {
		res = "POR/PDR";
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {
		res = "SOFT";
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {
		res = "IWDG";
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) {
		res = "WWDG";
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) {
		res = "LPWR";
	}
	else {
		res = "PIN";
	}

	if (clear) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
	}

	return res;
}

void internalFlashUnlock() {
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}

void internalFlashLock() {
	HAL_FLASH_Lock();
}

#define SECTOR_0_END  (0x08003FFF)
#define SECTOR_1_END  (0x08007FFF)
#define SECTOR_2_END  (0x0800BFFF)
#define SECTOR_3_END  (0x0800FFFF)
#define SECTOR_4_END  (0x0801FFFF)
#define SECTOR_5_END  (0x0803FFFF)
#define SECTOR_6_END  (0x0805FFFF)
#define SECTOR_7_END  (0x0807FFFF)
#define CR_PG_Set_    ((uint32_t)0x00000001)
#define CR_PG_Reset_  ((uint32_t)0x00001FFE)

void internalFlashEraseSectorCal(uint32_t address, uint32_t len) {
	if (address <= SECTOR_1_END) return;

	uint32_t address_end = address + len;

	if (address <= SECTOR_2_END && address_end > SECTOR_1_END) {
		printf("sector 2\n");
		FLASH_Erase_Sector(FLASH_SECTOR_2, FLASH_VOLTAGE_RANGE_3);
		delayBlocking(10);
	}
	if (address <= SECTOR_3_END && address_end > SECTOR_2_END) {
		printf("sector 3\n");
		FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);
		delayBlocking(10);
	}
	if (address <= SECTOR_4_END && address_end > SECTOR_3_END) {
		printf("sector 4\n");
		FLASH_Erase_Sector(FLASH_SECTOR_4, FLASH_VOLTAGE_RANGE_3);
		delayBlocking(10);
	}
	if (address <= SECTOR_5_END && address_end > SECTOR_4_END) {
		printf("sector 5\n");
		FLASH_Erase_Sector(FLASH_SECTOR_5, FLASH_VOLTAGE_RANGE_3);
		delayBlocking(10);
	}
	if (address <= SECTOR_6_END && address_end > SECTOR_5_END) {
		printf("sector 6\n");
		FLASH_Erase_Sector(FLASH_SECTOR_6, FLASH_VOLTAGE_RANGE_3);
		delayBlocking(10);
	}
	if (address <= SECTOR_7_END && address_end > SECTOR_6_END) {
		printf("sector 7\n");
		FLASH_Erase_Sector(FLASH_SECTOR_7, FLASH_VOLTAGE_RANGE_3);
		delayBlocking(10);
	}
}

void internalFlashWriteCal(uint32_t address, uint8_t *data, uint32_t len) {
	uint32_t temp;
	uint32_t index = 0;

	while (index < len) {
		temp = 0;
		memcpy(&temp, &data[index], (len - index) >= sizeof(uint32_t) ? sizeof(uint32_t) : (len - index));
		if (HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + index, temp)) {
			index += sizeof(uint32_t);
		}
		else {
			__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
		}
	}
}

/*
 * System core function
 */

void sysReset() {
	NVIC_SystemReset();
}

void delayBlockingUs(uint32_t us) {
	volatile uint32_t countUs = us;
	while (countUs--) {
		volatile uint64_t wait1us = 8;
		while (wait1us--) {
			;
		}
	}
}

void delayBlocking(uint32_t ms) {
	volatile uint32_t countMs = ms;
	while (countMs--) {
		volatile uint64_t wait1ms = 16000;
		while (wait1ms--) {
			;
		}
	}
}

void blinkLedLife(uint32_t ms) {
	toggleLedLife();
	delayBlocking(ms);
}

void blinkLedLife12(uint32_t ms1, uint32_t ms2) {
	setLedLife(false);
	delayBlocking(ms1);
	setLedLife(true);
	delayBlocking(ms2);
}

void refresh_wdg() {
	HAL_IWDG_Refresh(&hiwdg);
}

uint8_t getConsoleChar() {
	uint8_t c = 0xFF;
	if (HAL_OK == HAL_UART_Receive(&huart3, &c, 1, 100)) {
		return c;
	}
	return 0xFF;

}
