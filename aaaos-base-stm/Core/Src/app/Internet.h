/*
 * Internet.h
 *
 *  Created on: Sep 17, 2022
 *      Author: thanhbinh89
 */

#ifndef SRC_APP_INTERNET_H_
#define SRC_APP_INTERNET_H_

#include "netif.h"
#include "ip4_addr.h"

class InternetHelper {
public:
	InternetHelper();
	virtual ~InternetHelper();

	void initialize(struct netif *netif);
	void setStaticIP(ip_addr_t ip, ip_addr_t mask, ip_addr_t gw);
	bool connected(char **ips = NULL);
private:
	struct netif *mnetif = NULL;
};

extern InternetHelper Internet;

#endif /* SRC_APP_INTERNET_H_ */
