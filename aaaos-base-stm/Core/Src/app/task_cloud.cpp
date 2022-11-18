#include <time.h>
#include <string.h>

#include "netif.h"
#include "ip_addr.h"
#include "dhcp.h"
#include "dns.h"
#include "sntp.h"
#include "mqtt.h"

#include "aaa.h"

#include "app.h"
#include "app_log.h"
#include "task_list.h"
#include "task_cloud.h"
#include "task_device.h"
#include "Internet.h"
#include "MqttConnection.h"

#define TAG "TaskCloud"

extern struct netif gnetif;

static bool dhcpIsStarted() {
	return netif_get_client_data(&gnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP) ? true : false;
}

static void dns_found_cb(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
	APP_LOGD(TAG, "dns_found_cb: %s - %s", name, ip4addr_ntoa(ipaddr));
}

void MqttConnection::onConnect(int rc) {
	if (rc == 0) {
		AAATaskPostMsg(AAA_TASK_CLOUD_ID, CLOUD_MQTT_ON_CONNECTED, NULL, 0);
	}
}

void parserIncomingMessage(MqttConnection::DataItem_t *incomItem) {
	incomItem->topic[incomItem->topicLen] = 0;
	incomItem->data[incomItem->dataLen] = 0;
	APP_LOGD(TAG, "Parser topic: %.*s, data length: %d", incomItem->topicLen, incomItem->topic, incomItem->dataLen);
	APP_LOGD(TAG, "%.*s", incomItem->dataLen, incomItem->data);
	//TODO: handle incoming msgs
}

void TaskCloudEntry(void *params) {
	AAAWaitAllTaskStarted();
	APP_LOGI(TAG, "started");

	ip_addr_t dnsAddr = IPADDR4_INIT_BYTES(8,8,8,8);
	dns_setserver(0, &dnsAddr);

	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");

	Internet.initialize(&gnetif);
	if (gAppSetting.useStaticIP) {
		if (dhcpIsStarted()) {
			dhcp_stop(&gnetif);
		}
		Internet.setStaticIP(gAppSetting.staticIp, gAppSetting.subnetMask, gAppSetting.gateway);
		APP_LOGI(TAG, "Set static ip: %s", ip4addr_ntoa(&gAppSetting.staticIp));
	}
	Mqtt.initialize((const char*) gMacAddrStr, (const char*) gMacAddrStr);

	AAATimerSet(AAA_TASK_CLOUD_ID, CLOUD_CHECK_INTERNET_CONNECTED, NULL, 0, CLOUD_CHECK_INTERNET_CONNECTED_INTERVAL, true);

	void *msg = NULL;
	uint32_t len = 0, sig = 0;
	uint32_t id = *(uint32_t*) params;

	while (AAATaskRecvMsg(id, &sig, &msg, &len)) {
		switch (sig) {
		case CLOUD_NETIF_ON_CHANGE_STATUS: {
			APP_LOGI(TAG, "CLOUD_NETIF_ON_CHANGE_STATUS");
			bool isUP = *(bool*) msg;
			if (isUP) {
				if (!gAppSetting.useStaticIP && !dhcpIsStarted()) {
					if (ERR_OK != dhcp_start(&gnetif)) {
						SYS_PRINTF("dhcp start fail\n");
					}
					else {
						SYS_PRINTF("dhcp start successful\n");
					}
				}

				if (!sntp_enabled()) {
					sntp_init();
				}
			}
			else {
				if (sntp_enabled()) {
					sntp_stop();
				}
				AAATaskPostMsg(AAA_TASK_CLOUD_ID, CLOUD_MQTT_FORCE_DISCONNECT, NULL, 0);
			}

		}
			break;

		case CLOUD_CHECK_INTERNET_CONNECTED: {
			//APP_LOGI(TAG, "CLOUD_CHECK_INTERNET_CONNECTED");
			if (Internet.connected()) {
				ip_addr_t hostAddr;
				if (ERR_OK == dns_gethostbyname(SERVER_MQTT_HOST, &hostAddr, dns_found_cb,
				NULL)) {
					if (!Mqtt.isConnected()) {
						APP_LOGI(TAG, "My ip: %s", ip4addr_ntoa(netif_ip4_addr(&gnetif)));
						APP_LOGI(TAG, "Connecting to: %s:%d", ip4addr_ntoa(&hostAddr), SERVER_MQTT_PORT);
						Mqtt.connectBroker(hostAddr, SERVER_MQTT_PORT);
					}
				}
			}
		}
			break;

		case CLOUD_MQTT_ON_CONNECTED: {
			APP_LOGI(TAG, "CLOUD_MQTT_ON_CONNECTED");
			char buf[32];
			int len = sprintf(buf, "{\"IP\":\"%s\"}", ip4addr_ntoa(netif_ip4_addr(&gnetif)));
			if (!Mqtt.sendQueueIsFull()) {
				MqttConnection::DataItem_t sendItem;
				sendItem.topic = (char*) "aaaos/topic/ip";
				sendItem.topicLen = strlen("aaaos/topic/ip");
				sendItem.data = (char*) buf;
				sendItem.dataLen = len;
				sendItem.inHeap = false;
				Mqtt.sendEnqueue(&sendItem);

				sendItem.topic = (char*) "aaaos/topic/ver";
				sendItem.topicLen = strlen(sendItem.topic);
				sendItem.data = (char*) "{\"currentFwTitle\":\"" APP_FW_TITLE "\",\"currentFwVersion\":\"" APP_FW_VERSION "\"}";
				sendItem.dataLen = strlen(sendItem.data);
				sendItem.inHeap = false;
				Mqtt.sendEnqueue(&sendItem);

				AAATaskPostMsg(AAA_TASK_CLOUD_ID, CLOUD_MQTT_SEND_QUEUE, NULL, 0);
			}

			//TODO: subscribe topics
			Mqtt.subscribe("aaaos/topic/abc", 0);

		}
			break;

		case CLOUD_MQTT_FORCE_DISCONNECT: {
			APP_LOGI(TAG, "CLOUD_MQTT_FORCE_DISCONNECT");
			if (Mqtt.isConnected()) {
				Mqtt.disconnect();
			}
		}
			break;

		case CLOUD_MQTT_ON_MESSAGE: {
			APP_LOGI(TAG, "CLOUD_MQTT_ON_MESSAGE");
			MqttConnection::DataItem_t incomItem;
			while (Mqtt.incomQueueIsAvailable()) {
				Mqtt.incomDequeue(&incomItem);
				parserIncomingMessage(&incomItem);
				if (incomItem.inHeap) {
					Mqtt.freeQueueItem(&incomItem);
				}
			}
		}
			break;

		case CLOUD_MQTT_SEND_QUEUE: {
			APP_LOGI(TAG, "CLOUD_MQTT_SEND_QUEUE");
			if (Internet.connected() && Mqtt.isConnected() && Mqtt.sendQueueIsAvailable()) {
				Mqtt.publishQueue();
			}
		}
			break;

		default:
			break;
		}

		AAAFreeMsg(msg);
	}
}

