/*
 * Internet.c
 *
 *  Created on: Sep 17, 2022
 *      Author: thanhbinh89
 */
#include "app.h"
#include "app_log.h"
#include "task_list.h"

#include "ip_addr.h"
#include "dns.h"

#include <Internet.h>

#define TAG "Internet"

void netif_status_callback(struct netif *netif) {
	bool isUp;
	if (netif_is_up(netif)) {
		APP_LOGV(TAG, "netif up");
		isUp = true;
	}
	else {
		APP_LOGV(TAG, "netif down");
		isUp = false;
	}
	AAATaskPostMsg(AAA_TASK_CLOUD_ID, CLOUD_NETIF_ON_CHANGE_STATUS, &isUp, sizeof(isUp));
}

InternetHelper::InternetHelper() {

}

InternetHelper::~InternetHelper() {

}

void InternetHelper::initialize(struct netif *netif) {
	mnetif = netif;
	netif_set_status_callback(mnetif, netif_status_callback);
}

void InternetHelper::setStaticIP(ip_addr_t ip, ip_addr_t mask, ip_addr_t gw) {
	if (0 != ip.addr) {
		netif_set_addr(mnetif, &ip, &mask, &gw);
	}
}

bool InternetHelper::connected(char **ips) {
	if (mnetif) {
		if (netif_is_up(mnetif)) {
			const ip4_addr_t ip4_addr = *netif_ip4_addr(mnetif);
			if (0 != ip4_addr.addr) {
				if (NULL != ips) {
					*ips = ip4addr_ntoa(&ip4_addr);
				}
				return true;
			}
		}
	}
	if (NULL != ips) {
		*ips = (char*) "----";
	}
	return false;
}

InternetHelper Internet;
