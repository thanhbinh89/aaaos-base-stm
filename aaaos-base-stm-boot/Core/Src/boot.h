/*
 * boot.h
 *
 *  Created on: Nov 10, 2022
 *      Author: binh
 */

#ifndef SRC_BOOT_H_
#define SRC_BOOT_H_

#define BOOT_VER "0.0.1"

#define APP_START_ADDR (0x08008000UL)

#define OTA_HDR_MAGIC_NUMBER	(0x1234ABCD)

enum {
	OTA_INITIATED,
	OTA_EXFLASH_AVAILABLE,
	OTA_COPY_DONE,
};

typedef struct tOtaFwHeader {
	uint32_t magicNumber;
	uint32_t binStartAddress;
	uint32_t binLength;
	uint8_t checksum8;
	uint8_t otaState;
} __attribute__((__packed__)) OtaFwHeader_t;

extern void boot_main(void);

#endif /* SRC_BOOT_H_ */
