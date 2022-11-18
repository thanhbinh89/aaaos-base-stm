#include "aaa.h"

#include "app.h"
#include "app_log.h"
#include "app_ota.h"
#include "task_list.h"
#include "task_cloud.h"

#include "utils.h"

#include "sys_io.h"

#include "flash.h"

#define TAG "TaskOta"
#define OTA_HDR_MAGIC_NUMBER	(0x1234ABCD)

static uint32_t writeNextAddress;
static uint32_t willWriteLength;
static uint8_t readFlashBuffer[FLASH_PAGE_SIZE];

void TaskOtaEntry(void *params) {
	AAAWaitAllTaskStarted();
	APP_LOGI(TAG, "started");

	AAATimerSet(AAA_TASK_OTA_ID, OTA_INIT, NULL, 0, 1000, false);

	void *msg = NULL;
	uint32_t len = 0, sig = 0;
	uint32_t id = *(uint32_t*) params;

	while (AAATaskRecvMsg(id, &sig, &msg, &len)) {
		switch (sig) {
		case OTA_INIT: {
			APP_LOGI(TAG, "OTA_INIT");

			OtaFwHeader_t otaFwHeader;
			flash_read(OTA_BINARY_FLASH_HEADER_ADDRESS, (uint8_t*) &otaFwHeader, sizeof(otaFwHeader));
			if (otaFwHeader.magicNumber == OTA_HDR_MAGIC_NUMBER) {
				if (otaFwHeader.otaState == OTA_COPY_DONE) {
					APP_LOGI(TAG, "firmware updated");

					/* update fw header */
					otaFwHeader.otaState = OTA_INITIATED;
					flash_erase_sector(OTA_BINARY_FLASH_HEADER_ADDRESS);
					flash_write(OTA_BINARY_FLASH_HEADER_ADDRESS, (uint8_t*) &otaFwHeader, sizeof(otaFwHeader));

					//TODO: report firmware updated
				}
				else if (otaFwHeader.otaState == OTA_INITIATED) {
					//TODO: report firmware info
				}
			}
		}
			break;

		case OTA_START: {
			APP_LOGI(TAG, "OTA_START");

			char host[64], uri[128];
			char *be, *en;
			int len;
			FOTAAssign_t *FOTAAssign = (FOTAAssign_t*) msg;

			if (strlen(FOTAAssign->url)) {
				be = strstr(FOTAAssign->url, "//");
				if (be) {
					be += 2;
					en = strstr(be, "/");
					if (en) {
						len = (uint32_t) en - (uint32_t) be;
						strncpy(host, be, len);
						host[len] = 0;
						strcpy(uri, en);

						APP_LOGD(TAG, "host: %s", host);
						APP_LOGD(TAG, "uri: %s", uri);
						//https://espitek.s3.ap-southeast-1.amazonaws.com/thingsboard/gateway-stm32f4.bin
						otaStart(host, 80, uri);
					}
				}
			}
		}
			break;

		case OTA_START_DOWNLOAD: {
			APP_LOGI(TAG, "OTA_START_DOWNLOAD");

			uint32_t *content_len = (uint32_t*) msg;
			int num = *content_len / FLASH_SECTOR_SIZE;
			num += (*content_len % FLASH_SECTOR_SIZE) ? 1 : 0;

			APP_LOGV(TAG,"flash_erase len: %ld, num: %d", *content_len, num);
			for (int i = 0; i < num; i++) {
				if (FLASH_DRIVER_OK != flash_erase_sector(OTA_BINARY_FLASH_START_ADDRESS + FLASH_SECTOR_ADDR(i))) {
					APP_LOGE(TAG, "flash_erase fail: %d", i);
					break;
				}
			}

			writeNextAddress = OTA_BINARY_FLASH_START_ADDRESS;
			willWriteLength = *content_len;

			//TODO: report state=DOWNLOADING
		}
			break;

		case OTA_DOWNLOADING: {
			APP_LOGI(TAG, "OTA_DOWNLOADING");

			uint8_t *downloadBuf = (uint8_t*) msg;
			uint32_t downloadLen = len;

			if (FLASH_DRIVER_OK != flash_write(writeNextAddress, downloadBuf, downloadLen)) {
				APP_LOGE(TAG, "flash_write fail: x%lx, len: %ld", writeNextAddress, downloadLen);
			}
			writeNextAddress += downloadLen;
		}
			break;

		case OTA_DONE_DOWNLOAD: {
			APP_LOGI(TAG, "OTA_DONE_DOWNLOAD");

			uint8_t *downloadSum8 = (uint8_t*) msg;
			uint32_t writeTotalLength = writeNextAddress - OTA_BINARY_FLASH_START_ADDRESS;
			uint8_t stt;

			if (willWriteLength == writeTotalLength) {

				/* Sum8 of Flash */
				uint8_t flashSum8 = 0;
				uint32_t pageNum = writeTotalLength / FLASH_PAGE_SIZE;
				uint32_t remainBytes = writeTotalLength % FLASH_PAGE_SIZE;
				for (uint i = 0; i < pageNum; i++) {
					flash_read(OTA_BINARY_FLASH_START_ADDRESS + FLASH_PAGE_ADDR(i), readFlashBuffer, FLASH_PAGE_SIZE);
					flashSum8 += sum8Buffer(readFlashBuffer, FLASH_PAGE_SIZE);
				}
				if (remainBytes) {
					flash_read(OTA_BINARY_FLASH_START_ADDRESS + FLASH_PAGE_ADDR(pageNum), readFlashBuffer, remainBytes);
					flashSum8 += sum8Buffer(readFlashBuffer, remainBytes);
				}

				/* Verify sum8 between download with flash*/
				if (flashSum8 == *downloadSum8) {
					APP_LOGI(TAG, "checksum8 successful: x%02x", flashSum8);
					//TODO: report state=VERIFIED

					OtaFwHeader_t otaFwHeader;
					otaFwHeader.magicNumber = OTA_HDR_MAGIC_NUMBER;
					otaFwHeader.binStartAddress = OTA_BINARY_FLASH_START_ADDRESS;
					otaFwHeader.binLength = writeTotalLength;
					otaFwHeader.checksum8 = flashSum8;
					otaFwHeader.otaState = OTA_EXFLASH_AVAILABLE;
					stt = flash_erase_sector(OTA_BINARY_FLASH_HEADER_ADDRESS);
					if (FLASH_DRIVER_OK == stt && FLASH_DRIVER_OK == flash_write(OTA_BINARY_FLASH_HEADER_ADDRESS, (uint8_t*) &otaFwHeader, sizeof(otaFwHeader))) {
						APP_LOGW(TAG, "reboot after %d ms", OTA_REBOOT_INTERVAL);
						//TODO: report state=UPDATING

						AAATimerSet(AAA_TASK_OTA_ID, OTA_DONE, NULL, 0, OTA_REBOOT_INTERVAL, false);
					}
					else {
						APP_LOGW(TAG, "flash_write header fail");
						//TODO: report state=FAILED
					}
				}
				else {
					APP_LOGW(TAG, "fail flashSum8: x%02x, downloadSum8: x%02x", flashSum8, *downloadSum8);
					//TODO: report state=FAILED
				}
			}
			else {
				APP_LOGW(TAG, "willWriteLength: %ld, writeTotalLength: %ld", willWriteLength, writeTotalLength);
				//TODO: report state=FAILED
			}
		}
			break;

		case OTA_DONE: {
			APP_LOGI(TAG, "OTA_DONE");
			sysReset();
		}
			break;

		default:
			break;
		}
		AAAFreeMsg(msg);
	}
}

