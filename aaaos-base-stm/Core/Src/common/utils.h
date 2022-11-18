#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

extern bool getJsonString(char *input, char *output, const char *key);
extern bool getJsonNumber(char *input, uint32_t *output, const char *key);
extern uint8_t sum8Buffer(uint8_t *buff, uint32_t len);
extern bool checkNTC1Alarm(float newSample);
extern bool checkNTC2Alarm(float newSample);

#ifdef __cplusplus
}
#endif

#endif
