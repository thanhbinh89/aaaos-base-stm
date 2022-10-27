/*
 * time_redirect.c
 *
 *  Created on: Oct 3, 2022
 *      Author: binh
 */
#include <time.h>
#include "rng.h"
#include "sys_io.h"

extern int _gettimeofday(struct timeval *tv, void *tzvp);
int _gettimeofday(struct timeval *tv, void *tzvp) {
	(void)tzvp;
	uint32_t sec;
	getTimestamp(&sec);
	tv->tv_sec = sec;
	tv->tv_usec = 0;
	return 0;
}

extern int rand(void);
int rand(void) {
	return HAL_RNG_GetRandomNumber(&hrng);
}
