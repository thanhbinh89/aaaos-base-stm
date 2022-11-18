#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "flash.h"
#include "sys_io.h"

#include "boot.h"

#define ESCAPE          "\033"
#define RED_COLOR       "[0;31m"
#define YELLOW_COLOR    "[0;33m"
#define GREEN_COLOR     "[0;32m"
#define CYAN_COLOR      "[0;36m"
#define RESET_COLOR     "[0m"

#define NORMAL_START_ADDRESS APP_START_ADDR

typedef void (*p_jump_func)(void);

static void jump_to_application();

void boot_main(void) {
	printf(ESCAPE RESET_COLOR "[BOOT] Version: %s\n", BOOT_VER);
	setLedLife(true);

	OtaFwHeader_t otaFwHeader;
	uint32_t external_fw_index = 0;
	uint8_t *internal_fw_addr;
	uint8_t in_flash_sum8 = 0;
	uint8_t fStt;

	if ((uint8_t) 'r' == getConsoleChar()) {
		printf("[BOOT] erase firmware header\n");

		bzero(&otaFwHeader, sizeof(otaFwHeader));
		otaFwHeader.magicNumber = OTA_HDR_MAGIC_NUMBER;
		fStt = flash_erase_sector(OTA_BINARY_FLASH_HEADER_ADDRESS);
		flash_write(OTA_BINARY_FLASH_HEADER_ADDRESS, (uint8_t*) &otaFwHeader, sizeof(otaFwHeader));

		printf("[BOOT] start application\n");
		setLedLife(false);
		jump_to_application();
	}
	else {

		flash_read(OTA_BINARY_FLASH_HEADER_ADDRESS, (uint8_t*) &otaFwHeader, sizeof(otaFwHeader));
		if (otaFwHeader.magicNumber == OTA_HDR_MAGIC_NUMBER && otaFwHeader.otaState == OTA_EXFLASH_AVAILABLE) {
			printf("[BOOT] ex-flash firmware available\n");

			printf("[BOOT] erase in-flash addr: x%lx, len: %ld\n", NORMAL_START_ADDRESS, otaFwHeader.binLength);
			__disable_irq();
			internalFlashUnlock();
			internalFlashEraseSectorCal(NORMAL_START_ADDRESS, otaFwHeader.binLength);
			__enable_irq();

			printf("[BOOT] copy firmware from external flash\n");
			while (external_fw_index < otaFwHeader.binLength) {
				uint32_t readW = 0;

				refresh_wdg();

				flash_read(otaFwHeader.binStartAddress + external_fw_index, (uint8_t*) &readW, sizeof(uint32_t));

				__disable_irq();
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, NORMAL_START_ADDRESS + external_fw_index, readW);
				__enable_irq();

				external_fw_index += sizeof(uint32_t);
			}
			/* calculate sum8 */
			internal_fw_addr = (uint8_t*) ((uint32_t) NORMAL_START_ADDRESS);
			for (uint32_t i = 0; i < otaFwHeader.binLength; i++) {
				in_flash_sum8 += *(internal_fw_addr + i);
			}

			if (in_flash_sum8 == otaFwHeader.checksum8) {
				printf("[BOOT] internal checksum8 correctly\n");

				otaFwHeader.otaState = OTA_COPY_DONE;
				fStt = flash_erase_sector(OTA_BINARY_FLASH_HEADER_ADDRESS);
				if (FLASH_DRIVER_OK == fStt && FLASH_DRIVER_OK == flash_write(OTA_BINARY_FLASH_HEADER_ADDRESS, (uint8_t*) &otaFwHeader, sizeof(otaFwHeader))) {
					printf("[BOOT] start application\n");
					jump_to_application();
				}
				else {
					printf("[BOOT] flash_write header fail addr: x%lx\n", OTA_BINARY_FLASH_HEADER_ADDRESS);
					delayBlocking(1000);
					blinkLedLife12(50, 50);
					blinkLedLife12(50, 50);
					sysReset();
				}
			}
			else {
				printf("[BOOT] internal checksum fail in_flash_sum8: x%02x, checksum8: x%02x\n", in_flash_sum8, otaFwHeader.checksum8);
				printf("[BOOT] rebooting...\n");
				delayBlocking(1000);
				blinkLedLife12(50, 50);
				blinkLedLife12(50, 50);
				blinkLedLife12(50, 50);
				sysReset();
			}

		}
		else {
			printf("[BOOT] unexpected status\n");
			printf("[BOOT] start application\n");
			setLedLife(false);
			jump_to_application();
		}
	}

	while (1) {
		;
	}
}

void jump_to_application() {
	volatile uint32_t normal_stack_pointer = (uint32_t) *(volatile uint32_t*) (NORMAL_START_ADDRESS);
	volatile uint32_t normal_jump_address = (uint32_t) *(volatile uint32_t*) (NORMAL_START_ADDRESS + 4);

	/* check update boot info to share boot data before jump to application */
//	update_boot_fw_info_to_share_boot();
	p_jump_func jump_to_normal = (p_jump_func) normal_jump_address;

	HAL_RCC_DeInit();
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;

	/* Disable interrupt */
	__disable_irq();

	//__HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();

	__DMB();

	/* update interrupt vertor table */
	/* Edit with system_stm32f4xx:SystemInit() of Application */
	//SCB->VTOR = NORMAL_START_ADDRESS;
	/* set stack pointer */
	__set_MSP(normal_stack_pointer);

	__DSB();

	/* jump to normal program */
	jump_to_normal();

	while (1)
		;
}
