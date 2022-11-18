/*
 * app_ota.h
 *
 *  Created on: Nov 4, 2022
 *      Author: binh
 */

#ifndef SRC_APP_APP_OTA_H_
#define SRC_APP_APP_OTA_H_

#include <stdint.h>

bool otaStart(const char *server_name, uint16_t port, const char *uri);

#endif /* SRC_APP_APP_OTA_H_ */
