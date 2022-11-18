/*
 * sys_dbg.c
 *
 *  Created on: Oct 11, 2022
 *      Author: binh
 */
#include "aaa.h"

#include "iwdg.h"
#include "usart.h"

#include "sys_io.h"
#include "sys_log.h"

#include "flash.h"

#define TAG "SYS"

static fatalLog_t fatalLog;

void fatalInit() {
	const char *reason = getRstReason(true);
	SYS_LOGI(TAG, "Reset reason: %s", reason);

	flash_read(FATAL_LOG_FLASH_ADDRESS, (uint8_t*) &fatalLog, sizeof(fatalLog));
	if (fatalLog.magicNumber != FATAL_LOG_MAGIC_NUMBER) {
		SYS_LOGW(TAG, "Reinit fatal log: %s", reason);
		memset(&fatalLog, 0, sizeof(fatalLog));
		fatalLog.magicNumber = FATAL_LOG_MAGIC_NUMBER;
	}
	else {
		fatalLog.restartTimes++;
	}
	strcpy(fatalLog.rstReason, reason);
	flash_erase_sector(FATAL_LOG_FLASH_ADDRESS);
	flash_write(FATAL_LOG_FLASH_ADDRESS, (uint8_t*) &fatalLog, sizeof(fatalLog));
}

void fatal(const char *s) {
	taskDISABLE_INTERRUPTS();
	HAL_IWDG_Refresh(&hiwdg);

	SYS_LOGE("FATAL", "%s", s);

	flash_read(FATAL_LOG_FLASH_ADDRESS, (uint8_t*) &fatalLog, sizeof(fatalLog));
	fatalLog.uptime = HAL_GetTick();
	strcpy(fatalLog.string, s);
	flash_erase_sector(FATAL_LOG_FLASH_ADDRESS);
	flash_write(FATAL_LOG_FLASH_ADDRESS, (uint8_t*) &fatalLog, sizeof(fatalLog));

#ifdef DEBUG
	while (1) {
		blinkLedLife(200);
		HAL_IWDG_Refresh(&hiwdg);
	}
#else
	delayBlocking(2000);
	sysReset();
#endif
}

