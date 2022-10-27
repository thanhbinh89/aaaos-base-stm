#include "aaa.h"

#include "app.h"
#include "app_log.h"
#include "task_list.h"
#include "task_ui.h"
#include "ui_screen.h"
#include "Internet.h"
#include "MqttConnection.h"

#include "view_render.h"
#include "screen_manager.h"

#define TAG "TaskUi"

scr_mng_t screen_ui;

void TaskUiEntry(void *params) {
	AAAWaitAllTaskStarted();
	APP_LOGI(TAG, "started");

	view_render_init();

	AAATimerSet(AAA_TASK_UI_ID, UI_UPDATE, NULL, 0, UI_UPDATE_INTERVAL, true);

	void *msg = NULL;
	uint32_t len = 0, sig = 0;
	uint32_t id = *(uint32_t*) params;

	while (AAATaskRecvMsg(id, &sig, &msg, &len)) {
		switch (sig) {
		case UI_UPDATE: {
			APP_LOGI(TAG, "UI_UPDATE");
			char *ips;
			const char *conns;
			char times[24];
			time_t now = time(0);

			if ((uint32_t) now >= 1664468205UL) {
				now += (7 * 60 * 60);
				struct tm *ti = gmtime(&now);
				strftime(times, sizeof(times), "Time %H:%M:%S %d/%m/%y", ti);
			}
			else {
				strcpy(times, "Time ERROR");
			}

			conns = "Conn ERROR";
			if (Internet.connected(&ips)) {
				if (Mqtt.isConnected()) {
					conns = "Conn OK";
				}
			}

			view_render->firstPage();
			do {
				view_render->setDefaultForegroundColor();
				view_render->setFont(u8g_font_5x8);

				view_render->drawStr(0, 7, "ATM-MON-GW " APP_FW_VERSION);
				view_render->drawLine(0, 10, DISPLAY_WIDTH, 10);

				view_render->drawStr(0, 22, times);

				view_render->drawStr(0, 34, "MAC  ");
				view_render->drawStr(25, 34, gMacAddrStr);

				view_render->drawStr(0, 46, "IP   ");
				view_render->drawStr(25, 46, ips);

				view_render->drawStr(0, 58, conns);
			}
			while (view_render->nextPage());

		}
			break;

		default:
			break;
		}
		AAAFreeMsg(msg);
	}
}

