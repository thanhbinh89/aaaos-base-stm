/*
 * utils.cpp
 *
 *  Created on: Nov 3, 2022
 *      Author: binh
 */

#include <string.h>
#include <stdlib.h>
#include "utils.h"

bool getJsonString(char *input, char *output, const char *key) {
	char *begin = NULL, *end = NULL;
	int length = 0;
	char keyString[16] =
		{ 0 };

	strcat(keyString, "\"");
	strcat(keyString, key);
	strcat(keyString, "\"");
	begin = strstr(input, keyString);
	if (begin) {
		begin += strlen(keyString) + 2; /* "key":"value" */
		end = strstr(begin, "\"");
		if (end) {
			length = (uint32_t) end - (uint32_t) begin;
			strncpy(output, begin, length);
			output[length] = 0;
			return true;
		}
	}
	return false;
}

bool getJsonNumber(char *input, uint32_t *output, const char *key) {
	char *begin = NULL, *end = NULL;
	int length = 0;
	char oBuff[11] =
		{ 0 };
	char keyString[16] =
		{ 0 };

	strcat(keyString, "\"");
	strcat(keyString, key);
	strcat(keyString, "\"");
	begin = strstr(input, keyString);
	if (begin) {
		begin += strlen(keyString) + 1; /* "key":value */
		end = strstr(begin, ",");
		if (end) {
			length = (uint32_t) end - (uint32_t) begin;
			strncpy(oBuff, begin, length);
			oBuff[length] = 0;
			*output = atol((const char*) oBuff);
			return true;
		}
	}
	return false;
}

uint8_t sum8Buffer(uint8_t *buff, uint32_t len) {
	uint8_t sum8 = 0;
	uint32_t idx = 0;
	while (idx < len) {
		sum8 += *(buff + idx);
		idx++;
	}
	return sum8;
}

bool checkNTC1Alarm(float newSample) {
	static float ntcValueWindow[10] =
		{ 0 };
	static int ntcWindowIndex = 0, ntcWindowLength = sizeof(ntcValueWindow) / sizeof(ntcValueWindow[0]);

	if (ntcWindowIndex == 0) {
		memset(ntcValueWindow, 0, sizeof(ntcValueWindow));
	}

	if (ntcWindowIndex == ntcWindowLength) {
		// Shift to left
		for (int i = 0; i < ntcWindowLength - 1; i++) {
			ntcValueWindow[i] = ntcValueWindow[i + 1];
		}
		// Push to end
		ntcValueWindow[ntcWindowLength - 1] = newSample;
		for (int i = 0; i < ntcWindowLength; i++) {
		}
		if ((ntcValueWindow[ntcWindowLength - 1] - ntcValueWindow[5] > 4.0) && (ntcValueWindow[5] - ntcValueWindow[0] > 4.0)) {
			return true;
		}
	}
	else {
		ntcValueWindow[ntcWindowIndex++] = newSample;
		if (ntcWindowIndex == ntcWindowLength) {
			if ((ntcValueWindow[ntcWindowLength - 1] - ntcValueWindow[5] > 4.0) && (ntcValueWindow[5] - ntcValueWindow[0] > 4.0)) {
				return true;
			}
		}
	}
	return false;
}

bool checkNTC2Alarm(float newSample) {
	static float ntcValueWindow[10] =
		{ 0 };
	static int ntcWindowIndex = 0, ntcWindowLength = sizeof(ntcValueWindow) / sizeof(ntcValueWindow[0]);

	if (ntcWindowIndex == 0) {
		memset(ntcValueWindow, 0, sizeof(ntcValueWindow));
	}

	if (ntcWindowIndex == ntcWindowLength) {
		// Shift to left
		for (int i = 0; i < ntcWindowLength - 1; i++) {
			ntcValueWindow[i] = ntcValueWindow[i + 1];
		}
		// Push to end
		ntcValueWindow[ntcWindowLength - 1] = newSample;
		for (int i = 0; i < ntcWindowLength; i++) {
		}
		if ((ntcValueWindow[ntcWindowLength - 1] - ntcValueWindow[5] > 4.0) && (ntcValueWindow[5] - ntcValueWindow[0] > 4.0)) {
			return true;
		}
	}
	else {
		ntcValueWindow[ntcWindowIndex++] = newSample;
		if (ntcWindowIndex == ntcWindowLength) {
			if ((ntcValueWindow[ntcWindowLength - 1] - ntcValueWindow[5] > 4.0) && (ntcValueWindow[5] - ntcValueWindow[0] > 4.0)) {
				return true;
			}
		}
	}
	return false;
}
