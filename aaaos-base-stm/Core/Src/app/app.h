#ifndef __APP_H__
#define __APP_H__

#include "aaa.h"
#include "ip_addr.h"

/*****************************************************************************/
/*  task AAA_TASK_CONSOLE define
 */
/*****************************************************************************/
/* define timer */
/* define signal */
enum {
	CONSOLE_READ_NTC,
	CONSOLE_READ_CT,
	CONSOLE_SEND_TEST,
};

/*****************************************************************************/
/*  task AAA_TASK_DEVICE define
 */
/*****************************************************************************/
/* define timer */
#define DEVICE_SHOW_LED_STATUS_INTERVAL			(1000)
/* define signal */
enum {
	DEVICE_SHOW_LED_STATUS = AAA_USER_DEFINE_SIG,
};

/*****************************************************************************/
/*  task AAA_TASK_CLOUD define
 */
/*****************************************************************************/
/* define timer */
#define CLOUD_CHECK_DHCP_STARTED_INTERVAL		(6000)
#define CLOUD_CHECK_INTERNET_CONNECTED_INTERVAL	(7000)
/* define signal */
enum {
	CLOUD_NETIF_ON_CHANGE_STATUS = AAA_USER_DEFINE_SIG,
	CLOUD_CHECK_INTERNET_CONNECTED,
	CLOUD_MQTT_ON_CONNECTED,
	CLOUD_MQTT_FORCE_DISCONNECT,
	CLOUD_MQTT_ON_MESSAGE,
	CLOUD_MQTT_SEND_QUEUE,
};

/*****************************************************************************/
/*  task AAA_TASK_OTA define
 */
/*****************************************************************************/
/* define timer */
#define OTA_REBOOT_INTERVAL	(3000)
/* define signal */
enum {
	OTA_INIT,
	OTA_START,
	OTA_START_DOWNLOAD,
	OTA_DOWNLOADING,
	OTA_DONE_DOWNLOAD,
	OTA_DONE
};

/*****************************************************************************/
/*  task AAA_TASK_UI define
 */
/*****************************************************************************/
/* define timer */
#define UI_UPDATE_INTERVAL	(1000)
/* define signal */
enum {
	UI_UPDATE,
};

/*****************************************************************************/
/*  global define variable
 */
/*****************************************************************************/
#define APP_FW_MODEL "AAAOS"
#define APP_FW_VERSION "1.0.0"
#ifdef DEBUG
#define APP_FW_TITLE APP_FW_MODEL"-DEBUG"
#else
#define APP_FW_TITLE APP_FW_MODEL"-PROD"
#endif

#define SERVER_MQTT_HOST "broker.hivemq.com"
#define SERVER_MQTT_PORT 1883

#define SAMPLE_CT_SENSOR			            	(2500)

typedef struct tAppSetting {
	uint32_t magicNumber;
	bool useStaticIP;
	ip_addr_t staticIp;
	ip_addr_t subnetMask;
	ip_addr_t gateway;
} AppSetting_t;

typedef struct tFOTAAssign {
	char title[32];
	char version[32];
	uint32_t size;
	char MD5Checksum[32];
	char url[256];
} FOTAAssign_t;

enum {
	OTA_INITIATED, OTA_EXFLASH_AVAILABLE, OTA_COPY_DONE,
};

typedef struct tOtaFwHeader {
	uint32_t magicNumber;
	uint32_t binStartAddress;
	uint32_t binLength;
	uint8_t checksum8;
	uint8_t otaState;
} __attribute__((__packed__)) OtaFwHeader_t;

extern AppSetting_t gAppSetting;
extern char gMacAddrStr[13];

#endif // __APP_H__
