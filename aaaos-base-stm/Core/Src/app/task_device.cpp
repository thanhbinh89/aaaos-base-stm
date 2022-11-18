#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "aaa.h"

#include "app.h"
#include "app_log.h"
#include "task_list.h"
#include "task_device.h"
#include "MqttConnection.h"
#include "app_bsp.h"

#include "button.h"
#include "thermistor.h"
#include "EmonLib.h"
#include "flash.h"

#include "sys_io.h"
#include "iwdg.h"

#define TAG "TaskDevice"

static void sendStaticMsgToServer(const char *topic, const char *data);
static void sendDynamicMsgToServer(char *topic, char *data, int dataLen);

void appBspCallback(void *b) {
	button_t *butt = (button_t*) b;
	if (butt->state == BUTTON_SW_STATE_SHORT_RELEASE_PRESS) {
		APP_LOGV(TAG, "BUTTON_SW_STATE_SHORT_RELEASE_PRESS");
	}
	else if (butt->state == BUTTON_SW_STATE_LONG_PRESS) {
		APP_LOGV(TAG, "BUTTON_SW_STATE_LONG_PRESS");
	}
}

void TaskDeviceEntry(void *params) {
	AAAWaitAllTaskStarted();
	APP_LOGI(TAG, "started");

	/* Start app timers */
	AAATimerSet(AAA_TASK_DEVICE_ID, DEVICE_SHOW_LED_STATUS, NULL, 0, DEVICE_SHOW_LED_STATUS_INTERVAL, true);

	/* Start app button, start polling timer */
	appBspStart();

	void *msg = NULL;
	uint32_t len = 0, sig = 0;
	uint32_t id = *(uint32_t*) params;

	while (AAATaskRecvMsg(id, &sig, &msg, &len)) {
		switch (sig) {
		case DEVICE_SHOW_LED_STATUS: {
			//APP_LOGI(TAG, "DEVICE_SHOW_LED_STATUS");
			HAL_IWDG_Refresh(&hiwdg);
			toggleLedLife();
		}
			break;

		default:
			break;
		}

		AAAFreeMsg(msg);
	}
}

void sendStaticMsgToServer(const char *topic, const char *data) {
	if (!Mqtt.sendQueueIsFull()) {
		MqttConnection::DataItem_t sendItem;
		sendItem.topic = (char*) topic;
		sendItem.topicLen = strlen(sendItem.topic);
		sendItem.data = (char*) data;
		sendItem.dataLen = strlen(sendItem.data);
		sendItem.inHeap = false;
		Mqtt.sendEnqueue(&sendItem);
		AAATaskPostMsg(AAA_TASK_CLOUD_ID, CLOUD_MQTT_SEND_QUEUE, NULL, 0);
	}
}

void sendDynamicMsgToServer(char *topic, char *data, int dataLen) {
	if (!Mqtt.sendQueueIsFull()) {
		MqttConnection::DataItem_t sendItem;
		sendItem.topic = topic;
		sendItem.topicLen = strlen(sendItem.topic);
		sendItem.data = data;
		sendItem.dataLen = dataLen;
		sendItem.inHeap = true;
		Mqtt.sendEnqueue(&sendItem);
		AAATaskPostMsg(AAA_TASK_CLOUD_ID, CLOUD_MQTT_SEND_QUEUE, NULL, 0);
	}
}
