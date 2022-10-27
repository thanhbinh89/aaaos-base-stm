/*
 * MqttConnection.h
 *
 *  Created on: Sep 18, 2022
 *      Author: thanhbinh89
 */

#ifndef SRC_APP_MQTTCONNECTION_H_
#define SRC_APP_MQTTCONNECTION_H_

#include "main.h"
#include "aaa.h"

#include "app_log.h"
#include "Internet.h"
#include "mqtt.h"

class MqttConnection {
public:
	typedef struct t_IncomItem {
		char *topic;
		int topicLen;
		char *data;
		int dataLen;
		bool inHeap;
	} DataItem_t;

	DataItem_t incomItemTmp;
	bool isPublishing;

	MqttConnection();
	virtual ~MqttConnection();
	void initialize(const char *clientId = "atm-client", const char *clientUser = "atm-user");
	bool connectBroker(ip_addr_t hostAddr, uint16_t port = 1880);
	void disconnect() {
		mqtt_disconnect(client);
		isPublishing = false;
	}
	;
	bool isConnected() {
		return mqtt_client_is_connected(client);
	}
	;
	void subscribe(const char *topic, int qos = 0);
	void publish(const char *topic, char *data, int dataLen, int qos = 0, int retain = 0);
	void publishQueue();
	bool incomQueueIsFull() {
		return 0 == (int) uxQueueSpacesAvailable(incomQueue);
	}
	;
	int incomQueueIsAvailable() {
		return (int) uxQueueMessagesWaiting(incomQueue);
	}
	;
	void incomEnqueue(DataItem_t *IncomItem) {
		xQueueSend(incomQueue, IncomItem, portMAX_DELAY);
	}
	;
	void incomDequeue(DataItem_t *IncomItem) {
		xQueueReceive(incomQueue, IncomItem, portMAX_DELAY);
	}
	;
	bool sendQueueIsFull() {
		return 0 == (int) uxQueueSpacesAvailable(sendQueue);
	}
	;
	int sendQueueIsAvailable() {
		return (int) uxQueueMessagesWaiting(sendQueue);
	}
	;
	void sendEnqueue(DataItem_t *SendItem) {
		xQueueSend(sendQueue, SendItem, portMAX_DELAY);
	}
	;
	void sendPeekQueue(DataItem_t *SendItem) {
		xQueuePeek(sendQueue, SendItem, portMAX_DELAY);
	}
	;
	void sendDequeue(DataItem_t *SendItem) {
		xQueueReceive(sendQueue, SendItem, portMAX_DELAY);
	}
	;
	void freeQueueItem(DataItem_t *SendItem) {
		vPortFree(SendItem->topic);
		vPortFree(SendItem->data);
	}
	;
	virtual void onConnect(int rc);

private:
	mqtt_client_t *client;
	struct mqtt_connect_client_info_t clientInfo;
	QueueHandle_t incomQueue, sendQueue;
};

extern MqttConnection Mqtt;

/*
 * TELEMETRY
 * Topic
 *  v1/devices/me/telemetry
 * Data publish
 *  {"key1":"value1", "key2":"value2"} or
 *  [{"key1":"value1"}, {"key2":"value2"}] or
 *  {"ts":1451649600512, "values":{"key1":"value1", "key2":"value2"}}
 *
 * ATTRIBUTES *
 * Publish attribute update to the server
 *  v1/devices/me/attributes
 * Request attribute values from the server
 *  v1/devices/me/attributes/request/$request_id
 *  v1/devices/me/attributes/response/+
 * Subscribe to attribute updates from the server
 *  v1/devices/me/attributes
 *  {"key1":"value1"}
 *
 * RPC
 * Subscribe to RPC commands from the server
 *  v1/devices/me/rpc/request/$request_id
 *  v1/devices/me/rpc/response/$request_id
 * Send RPC commands to server
 *  v1/devices/me/rpc/request/$request_id
 *  v1/devices/me/rpc/response/$request_id
 *
 * */

#endif /* SRC_APP_MQTTCONNECTION_H_ */
