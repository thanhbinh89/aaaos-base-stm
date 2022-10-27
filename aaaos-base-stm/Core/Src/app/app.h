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
#define APP_MANUFACTURER "AAAOS"
#define APP_MODEL "BASE-01"
#define APP_FIRMWARE "AAAOS-BASE"
#define APP_FW_VERSION "0.0.1"

#define SERVER_MQTT_HOST "broker.hivemq.com"
#define SERVER_MQTT_PORT 1883

#define SAMPLE_CT_SENSOR			            	(2500)

#define APP_FLASH_MAGIC_NUMBER  		(0xB1B2E3E4)
#define APP_SETTING_FLASH_ADDRESS		FLASH_SECTOR_ADDR(1)
#define PERSISTANCE_FLAGS_FLASH_ADDRESS	FLASH_SECTOR_ADDR(2)

typedef struct tAppSetting {
	uint32_t magicNumber;
	bool useStaticIP;
	ip_addr_t staticIp;
	ip_addr_t subnetMask;
	ip_addr_t gateway;
} AppSetting_t;

extern AppSetting_t gAppSetting;
extern char gMacAddrStr[13];

#endif // __APP_H__
