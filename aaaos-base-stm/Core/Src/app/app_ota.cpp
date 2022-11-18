/*
 * app_ota.cpp
 *
 *  Created on: Nov 4, 2022
 *      Author: binh
 */

#include "aaa.h"

#include "app.h"
#include "app_log.h"
#include "app_ota.h"
#include "task_list.h"

#include "utils.h"

#include "flash.h"
#include "http_client.h"

#define TAG "OTA"

static httpc_connection_t httpc_connection =
	{ 0 };
static httpc_state_t *httpc_state = NULL;
static uint8_t downloadSum8;
bool downloadStarted = false;

static err_t httpc_headers_done_cb(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len) {
	//APP_LOGV(TAG, "header %.*s", hdr->len, (char *)hdr->payload);
	APP_LOGV(TAG, "headers_done_cb content_len: %lu", content_len);
	if ((int32_t) content_len != -1) {
		downloadSum8 = 0;
		AAATaskPostMsg(AAA_TASK_OTA_ID, OTA_START_DOWNLOAD, &content_len, sizeof(content_len));
		downloadStarted = true;
	}
	return ERR_OK;
}

static err_t tcp_recv_cb(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
	if (p) {
		//APP_LOGV(TAG, "recv_cb len: %d, err: %d", p->len, err);
		if (downloadStarted) {
			downloadSum8 += sum8Buffer((uint8_t*) p->payload, p->len);
			AAATaskPostMsg(AAA_TASK_OTA_ID, OTA_DOWNLOADING, p->payload, p->len);
		}
		altcp_recved(tpcb, p->tot_len);
		pbuf_free(p);
		return ERR_OK;
	}
	return ERR_ABRT;
}

static void httpc_result_cb(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err) {
	APP_LOGV(TAG, "result_cb result: %d, content_len: %ld, res: %lu, err: %d", httpc_result, (int32_t)rx_content_len, srv_res, err);
	if (err == ERR_OK && downloadStarted) {
		AAATaskPostMsg(AAA_TASK_OTA_ID, OTA_DONE_DOWNLOAD, &downloadSum8, sizeof(downloadSum8));
	}
	downloadStarted = false;
}

bool otaStart(const char *server_name, uint16_t port, const char *uri) {
	bzero(&httpc_connection, sizeof(httpc_connection));
	httpc_connection.use_proxy = 0;
	httpc_connection.headers_done_fn = httpc_headers_done_cb;
	httpc_connection.result_fn = httpc_result_cb;
	return ERR_OK == httpc_get_file_dns(server_name, port, uri, &httpc_connection, tcp_recv_cb, NULL, &httpc_state);
}
