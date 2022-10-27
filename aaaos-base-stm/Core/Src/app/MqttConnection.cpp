/*
 * MqttConnection.cpp
 *
 *  Created on: Sep 18, 2022
 *      Author: thanhbinh89
 */
#include "aaa.h"

#include "app.h"
#include "app_log.h"
#include "task_list.h"

#include "ip4_addr.h"

#include <MqttConnection.h>

#define TAG "MQTT"

extern "C" void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
	APP_LOGD(TAG, "connection_status: %d", (int)status);
	if (arg != NULL) {
		MqttConnection *mqtt = (MqttConnection*) arg;
		mqtt->onConnect((int) status);
	}
}

extern "C" void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) {
	APP_LOGD(TAG,"Incoming topic %s", topic);
	if (arg != NULL) {
		MqttConnection *mqtt = (MqttConnection*) arg;
		if (!mqtt->incomQueueIsFull()) {
			int tLen = strlen(topic);
			char *tBuf = (char*) pvPortMalloc(tLen);
			if (tBuf) {
				memcpy(tBuf, topic, tLen);
			}
			mqtt->incomItemTmp.topic = tBuf;
			mqtt->incomItemTmp.topicLen = tLen;
			mqtt->incomItemTmp.inHeap = true;
		}
	}
}

extern "C" void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
	APP_LOGD(TAG,"Incoming payload length %d, flags %u", len, (unsigned int) flags);
	if (arg != NULL) {
		MqttConnection *mqtt = (MqttConnection*) arg;
		if (flags & MQTT_DATA_FLAG_LAST) {
			/* Last fragment of payload received (or whole part if payload fits receive buffer
			 See MQTT_VAR_HEADER_BUFFER_LEN)  */
			if (mqtt->incomItemTmp.topic) {
				char *dBuf = (char*) pvPortMalloc(len);
				if (dBuf) {
					memcpy(dBuf, data, len);
					mqtt->incomItemTmp.data = dBuf;
					mqtt->incomItemTmp.dataLen = len;
					mqtt->incomEnqueue(&(mqtt->incomItemTmp));

					AAATaskPostMsg(AAA_TASK_CLOUD_ID, CLOUD_MQTT_ON_MESSAGE, NULL, 0);
				}
				else {
					vPortFree(mqtt->incomItemTmp.topic);
				}
			}
		}
		else {
			/* Handle fragmented payload, store in buffer, write to file or whatever */
		}
	}
}

extern "C" void mqtt_sub_request_cb(void *arg, err_t result) {
	/* Just print the result code here for simplicity,
	 normal behaviors would be to take some action if subscribe fails like
	 notifying user, retry subscribe or disconnect from server */
	APP_LOGD(TAG,"Subscribe result: %s", result == 0 ? "done" : "fail");
}

/* Called when publish is complete either with success or failure */
extern "C" void mqtt_pub_request_cb(void *arg, err_t result) {
	APP_LOGD(TAG,"Publish result: %s", result == 0 ? "done" : "fail");
	if (arg != NULL) {
		MqttConnection *mqtt = (MqttConnection*) arg;
		if (result == 0) {
			MqttConnection::DataItem_t item =
				{ 0 };
			mqtt->sendDequeue(&item);
			if (item.inHeap) {
				mqtt->freeQueueItem(&item);
			}

			AAATaskPostMsg(AAA_TASK_CLOUD_ID, CLOUD_MQTT_SEND_QUEUE, NULL, 0);
		}
		else {
			AAATaskPostMsg(AAA_TASK_CLOUD_ID, CLOUD_MQTT_FORCE_DISCONNECT, NULL, 0);
		}
		mqtt->isPublishing = false;
	}
}

MqttConnection::MqttConnection() {
	client = NULL;
	incomQueue = NULL;
	sendQueue = NULL;
	isPublishing = false;
}

MqttConnection::~MqttConnection() {
}

void MqttConnection::initialize(const char *clientId, const char *clientUser) {
	memset(&clientInfo, 0, sizeof(clientInfo));
	memset(&incomItemTmp, 0, sizeof(incomItemTmp));
	client = mqtt_client_new();
	if (client) {
		incomQueue = xQueueCreate(5, sizeof(DataItem_t));
		sendQueue = xQueueCreate(32, sizeof(DataItem_t));
		clientInfo.client_id = clientId;
		clientInfo.client_user = clientUser;
	}
}

bool MqttConnection::connectBroker(ip_addr_t hostAddr, uint16_t port) {
	isPublishing = false;
	if (ERR_OK != mqtt_client_connect(client, &hostAddr, port, mqtt_connection_cb, this, &clientInfo)) {
		mqtt_disconnect(client);
		return false;
	}
	mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, this);
	return true;
}

void MqttConnection::subscribe(const char *topic, int qos) {
	mqtt_subscribe(client, topic, (uint8_t )qos, mqtt_sub_request_cb, this);
	APP_LOGD(TAG, "Subscribe topic: %s", topic);
}

void MqttConnection::publish(const char *topic, char *data, int dataLen, int qos, int retain) {
	mqtt_publish(client, topic, data, dataLen, (uint8_t) qos, (uint8_t) retain, mqtt_pub_request_cb, NULL);
	APP_LOGD(TAG, "Publish topic: %s, data length: %d", topic, dataLen);
	//APP_LOGD(TAG, "%.*s", dataLen, data);
}

void MqttConnection::publishQueue() {
	if (!isPublishing) {
		isPublishing = true;
		DataItem_t item =
			{ 0 };
		sendPeekQueue(&item);
		mqtt_publish(client, item.topic, item.data, item.dataLen, 1, 0, mqtt_pub_request_cb, this);
		APP_LOGD(TAG, "Publish topic: %s, data length: %d", item.topic, item.dataLen);
	}
}

MqttConnection Mqtt;
