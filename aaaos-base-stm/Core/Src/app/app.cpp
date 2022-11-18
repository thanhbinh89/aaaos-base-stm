#include "aaa.h"

#include "app.h"
#include "app_log.h"

#include "flash.h"

#include "sys_log.h"
#include "sys_io.h"

#define APP_SETTING_MAGIC_NUMBER (0xB1B2E3E4)
#define MAC_FORMAT "%02x%02x%02x%02x%02x%02x"
#define MAC_SPLIT(m) m[0], m[1], m[2], m[3], m[4], m[5]

#define TAG "App"

AppSetting_t gAppSetting =
	{ 0 };
char gMacAddrStr[13] =
	{ 0 };

static void appSettingInit() {
	flash_read(APP_SETTING_FLASH_ADDRESS, (uint8_t*) &gAppSetting, sizeof(gAppSetting));
	if (gAppSetting.magicNumber != APP_SETTING_MAGIC_NUMBER) {
		APP_LOGW(TAG, "App setting invalid");
		memset(&gAppSetting, 0, sizeof(gAppSetting));
		gAppSetting.magicNumber = APP_SETTING_MAGIC_NUMBER;
		gAppSetting.useStaticIP = false;
		flash_erase_sector(APP_SETTING_FLASH_ADDRESS);
		flash_write(APP_SETTING_FLASH_ADDRESS, (uint8_t*) &gAppSetting, sizeof(gAppSetting));
	}
}

void AAATaskInit() {
	SYS_LOGI(TAG, "App version: " APP_FW_VERSION);
#ifdef DEBUG
	SYS_LOGW(TAG, "Build type: DEBUG");
#else
	SYS_LOGI(TAG, "Build type: RELEASE");
#endif

	uint8_t macAddr[6];
	uint32_t UIDw0 = HAL_GetUIDw0();
	uint32_t UIDw1 = HAL_GetUIDw1();
	uint32_t UIDw2 = HAL_GetUIDw2();
	macAddr[0] = 0x00;
	macAddr[1] = 0x80;
	macAddr[2] = 0xE1;
	macAddr[3] = ((UIDw0 & 0xFF) ^ (UIDw0 >> 8 & 0xFF) ^ (UIDw0 >> 16 & 0xFF) ^ (UIDw0 >> 24 & 0xFF));
	macAddr[4] = ((UIDw1 & 0xFF) ^ (UIDw1 >> 8 & 0xFF) ^ (UIDw1 >> 16 & 0xFF) ^ (UIDw1 >> 24 & 0xFF));
	macAddr[5] = ((UIDw2 & 0xFF) ^ (UIDw2 >> 8 & 0xFF) ^ (UIDw2 >> 16 & 0xFF) ^ (UIDw2 >> 24 & 0xFF));
	sprintf(gMacAddrStr, MAC_FORMAT, MAC_SPLIT(macAddr));
	APP_LOGI(TAG, "MAC address: %s", gMacAddrStr);

	fatalInit();
	appSettingInit();
}
