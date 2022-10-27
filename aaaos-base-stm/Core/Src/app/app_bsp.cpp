/*
 * app_bsp.cpp
 *
 *  Created on: Oct 4, 2022
 *      Author: binh
 */

#include "aaa.h"

#include "app.h"
#include "app_log.h"
#include "task_list.h"
#include "task_device.h"

#include "sys_io.h"

#include "button.h"

#define TAG "BSP"

#define BSP_POLLING_INTERVAL	(50)

button_t offBtn, manBtn, alrmBtn, autoBtn;
button_t upBtn, leftBtn, rightBtn, downBtn, setBtn;
button_t rl1Btn, rl2Btn, rl3Btn, rl4Btn;

static void timerCallback(TimerHandle_t timHandler) {
	shiftInput();
	button_timer_polling(&offBtn);
	button_timer_polling(&manBtn);
	button_timer_polling(&alrmBtn);
	button_timer_polling(&autoBtn);
	button_timer_polling(&upBtn);
	button_timer_polling(&leftBtn);
	button_timer_polling(&rightBtn);
	button_timer_polling(&downBtn);
	button_timer_polling(&setBtn);
	button_timer_polling(&rl1Btn);
	button_timer_polling(&rl2Btn);
	button_timer_polling(&rl3Btn);
	button_timer_polling(&rl4Btn);
}

static uint8_t readInputU8(uint8_t i) {
	return getInput(i) ? 1 : 0;
}

void appBspStart() {

	button_init(&offBtn, BSP_POLLING_INTERVAL, OFF_BTN_IDX, NULL, readInputU8, appBspCallback);
	button_init(&manBtn, BSP_POLLING_INTERVAL, MANUAL_BTN_IDX, NULL, readInputU8, appBspCallback);
	button_init(&alrmBtn, BSP_POLLING_INTERVAL, ALARM_BTN_IDX, NULL, readInputU8, appBspCallback);
	button_init(&autoBtn, BSP_POLLING_INTERVAL, AUTO_BTN_IDX, NULL, readInputU8, appBspCallback);
	button_init(&upBtn, BSP_POLLING_INTERVAL, UP_BTN_IDX, NULL, readInputU8, appBspCallback);
	button_init(&leftBtn, BSP_POLLING_INTERVAL, LEFT_BTN_IDX, NULL, readInputU8, appBspCallback);
	button_init(&rightBtn, BSP_POLLING_INTERVAL, RIGHT_BTN_IDX, NULL, readInputU8, appBspCallback);
	button_init(&downBtn, BSP_POLLING_INTERVAL, DOWN_BTN_IDX, NULL, readInputU8, appBspCallback);
	button_init(&setBtn, BSP_POLLING_INTERVAL, SET_BTN_IDX, NULL, readInputU8, appBspCallback);
	button_init(&rl1Btn, BSP_POLLING_INTERVAL, RL1_BTN_IDX, NULL, readInputU8, appBspCallback);
	button_init(&rl2Btn, BSP_POLLING_INTERVAL, RL2_BTN_IDX, NULL, readInputU8, appBspCallback);
	button_init(&rl3Btn, BSP_POLLING_INTERVAL, RL3_BTN_IDX, NULL, readInputU8, appBspCallback);
	button_init(&rl4Btn, BSP_POLLING_INTERVAL, RL4_BTN_IDX, NULL, readInputU8, appBspCallback);
	button_enable(&offBtn);
	button_enable(&manBtn);
	button_enable(&alrmBtn);
	button_enable(&autoBtn);
	button_enable(&upBtn);
	button_enable(&leftBtn);
	button_enable(&rightBtn);
	button_enable(&downBtn);
	button_enable(&setBtn);
	button_enable(&rl1Btn);
	button_enable(&rl2Btn);
	button_enable(&rl3Btn);
	button_enable(&rl4Btn);

	TimerHandle_t timHandler = xTimerCreate("appBsp", pdMS_TO_TICKS(BSP_POLLING_INTERVAL), pdTRUE, NULL, timerCallback);
	if (timHandler != NULL) {
		xTimerStart(timHandler, pdMS_TO_TICKS(BSP_POLLING_INTERVAL));
	}
}

