/*
 * sys_io.h
 *
 *  Created on: Sep 26, 2022
 *      Author: binh
 */

#ifndef SRC_SYS_SYS_IO_H_
#define SRC_SYS_SYS_IO_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FATAL_LOG_FLASH_ADDRESS			FLASH_SECTOR_ADDR(255)
#define APP_SETTING_FLASH_ADDRESS		FLASH_SECTOR_ADDR(254)
#define PERSISTANCE_FLAGS_FLASH_ADDRESS	FLASH_SECTOR_ADDR(253)
#define OTA_BINARY_FLASH_HEADER_ADDRESS	FLASH_SECTOR_ADDR(0)
#define OTA_BINARY_FLASH_START_ADDRESS	FLASH_SECTOR_ADDR(1)

#define millis()						HAL_GetTick()

extern void setBuzzer(bool on);
extern void setBuzzerToggle();
extern void setLedLife(bool on);
extern void toggleLedLife();
extern void setLedStatus(bool on);
extern void setFlashSpiCS(bool on);
extern uint8_t tranferFlashSpi(uint8_t send);
extern const char* getRstReason(bool clear);
extern void internalFlashUnlock();
extern void internalFlashLock();
extern void internalFlashEraseSectorCal(uint32_t address, uint32_t len);
extern void internalFlashWriteCal(uint32_t address, uint8_t *data, uint32_t len);
/*
 * System core function
 */
extern void sysReset();
extern void delayBlockingUs(uint32_t us);
extern void delayBlocking(uint32_t ms);
extern void blinkLedLife(uint32_t ms);
extern void blinkLedLife12(uint32_t ms1, uint32_t ms2);
extern void setLedLife(bool on);
extern void toggleLedLife();
extern void refresh_wdg();
extern uint8_t getConsoleChar();
#ifdef __cplusplus
}
#endif

#endif /* SRC_SYS_SYS_IO_H_ */
