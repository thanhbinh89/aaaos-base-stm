#include "aaa.h"

#include "app.h"
#include "app_log.h"
#include "task_list.h"
#include "task_console.h"
#include "MqttConnection.h"

#include "thermistor.h"
#include "EmonLib.h"

#include "sys_io.h"

#include "usart.h"
#include "stm32f4xx_hal_uart.h"

#define TAG "TaskConsole"

#define RX_QUEUE_SIZE						(256)
#define CONSOLE_BUFFER_LENGHT				RX_QUEUE_SIZE
struct console_t {
	uint8_t rxChar;
	uint8_t index;
	uint8_t data[CONSOLE_BUFFER_LENGHT];
} console;
static QueueHandle_t rxQueue = NULL;

void consoleInputTask(void *params) {
	uint8_t c;

	HAL_UART_Receive_IT(&huart3, &console.rxChar, 1);
	while (1) {
		if (pdFAIL != xQueueReceive(rxQueue, &c, portMAX_DELAY)) {
			if (console.index < CONSOLE_BUFFER_LENGHT - 1) {
				if (c == '\r' || c == '\n') {
					console.data[console.index] = c;
					console.data[console.index + 1] = 0;
					{
						/* Parser with table */
						switch (cmd_line_parser(lgn_cmd_table, console.data)) {
						case CMD_SUCCESS:
							break;

						case CMD_NOT_FOUND:
							if (console.data[0] != '\r' && console.data[0] != '\n') {
								SYS_PRINTF("cmd unknown\n");
							}
							break;

						case CMD_TOO_LONG:
							SYS_PRINTF("cmd too long\n");
							break;

						case CMD_TBL_NOT_FOUND:
							SYS_PRINTF("cmd table not found\n");
							break;

						default:
							SYS_PRINTF("cmd error\n");
							break;
						}
					}
					SYS_PRINTF("#\n");

					console.index = 0;
				}
				else if (c == 8) {
					if (console.index) {
						console.index--;
					}
				}
				else {
					console.data[console.index++] = c;
				}
			}
			else {
				SYS_PRINTF("\nerror: cmd too long, cmd size: %d, try again !\n", CONSOLE_BUFFER_LENGHT);
				console.index = 0;
			}
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART3) {
		if (rxQueue) {
			if (!xQueueIsQueueFullFromISR(rxQueue)) {
				BaseType_t xHigherPriorityTaskWoken = pdFALSE;
				xQueueSendFromISR(rxQueue, &console.rxChar, &xHigherPriorityTaskWoken);
				if (xHigherPriorityTaskWoken == pdTRUE) {
					portYIELD();
				}
			}
			HAL_UART_Receive_IT(&huart3, &console.rxChar, 1);
		}
	}
}

static THERMISTOR thermistor[2];
static EnergyMonitor ctSensor[4];

void TaskConsoleEntry(void *params) {
	AAAWaitAllTaskStarted();
	APP_LOGI(TAG, "started");

	thermistor[0].begin(ADC_CHANNEL_8, 10000, 3590, 10000);
	thermistor[1].begin(ADC_CHANNEL_9, 10000, 3590, 10000);
	ctSensor[0].current(ADC_CHANNEL_3, (double) 2000 / (double) 100);
	ctSensor[1].current(ADC_CHANNEL_4, (double) 2000 / (double) 100);
	ctSensor[2].current(ADC_CHANNEL_5, (double) 2000 / (double) 100);
	ctSensor[3].current(ADC_CHANNEL_6, (double) 2000 / (double) 100);

	rxQueue = xQueueCreate(RX_QUEUE_SIZE, sizeof(uint8_t));
	xTaskCreate(consoleInputTask, "conInp", AAA_TASK_DEPTH_MEDIUM, NULL, AAA_TASK_PRIORITY_MAX, NULL);

	void *msg = NULL;
	uint32_t len = 0, sig = 0;
	uint32_t id = *(uint32_t*) params;

	while (AAATaskRecvMsg(id, &sig, &msg, &len)) {
		switch (sig) {
		case CONSOLE_READ_NTC: {
			APP_LOGI(TAG, "CONSOLE_READ_NTC");
			float temp1 = thermistor[0].read_f();
			float temp2 = thermistor[1].read_f();
			APP_LOGD(TAG,"Temp 1: %0.1f", temp1);
			APP_LOGD(TAG,"Temp 2: %0.1f", temp2);
		}
			break;

		case CONSOLE_READ_CT: {
			APP_LOGI(TAG, "CONSOLE_READ_CT");
			uint16_t mAVal1 = (uint16_t) (1000.0 * ctSensor[0].calcIrms(SAMPLE_CT_SENSOR));
			uint16_t mAVal2 = (uint16_t) (1000.0 * ctSensor[1].calcIrms(SAMPLE_CT_SENSOR));
			uint16_t mAVal3 = (uint16_t) (1000.0 * ctSensor[2].calcIrms(SAMPLE_CT_SENSOR));
			uint16_t mAVal4 = (uint16_t) (1000.0 * ctSensor[3].calcIrms(SAMPLE_CT_SENSOR));
			APP_LOGD(TAG,"CT 1: %d", mAVal1);
			APP_LOGD(TAG,"CT 2: %d", mAVal2);
			APP_LOGD(TAG,"CT 3: %d", mAVal3);
			APP_LOGD(TAG,"CT 4: %d", mAVal4);
		}
			break;

		case CONSOLE_SEND_TEST: {
			APP_LOGI(TAG, "CONSOLE_SEND_TEST");
			MqttConnection::DataItem_t sendItem;
			sendItem.topic = (char *)"aaaos/topic/test";
			sendItem.topicLen = strlen(sendItem.topic);
			sendItem.data = (char *)"{\"Test\":true}";
			sendItem.dataLen = strlen(sendItem.data);
			sendItem.inHeap = false;
			Mqtt.sendEnqueue(&sendItem);
		}
			break;

		default:
			break;
		}
		AAAFreeMsg(msg);
	}
}

