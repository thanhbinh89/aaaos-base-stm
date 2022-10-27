#ifndef __SENSOR_TRISTATE_H__
#define __SENSOR_TRISTATE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define THRESHOLD_IGNORED	(0xFFFF)
#define TIME_IGNORED		(0xFFFFFFFF)

typedef enum e_sensor_tristate_status {
	SENSOR_TRISTATE_STATUS_SUCCESS,
	SENSOR_TRISTATE_STATUS_FAIL
} sensor_tristate_status_t;

typedef enum e_sensor_tristate_state {
	SENSOR_TRISTATE_STATE_UNKNOWN,
	SENSOR_TRISTATE_STATE_LOW,
	SENSOR_TRISTATE_STATE_MEDIUM,
	SENSOR_TRISTATE_STATE_HIGH
} sensor_tristate_state_t;

typedef void (*pf_sensor_tristate_ctrl)();
typedef uint16_t (*pf_sensor_tristate_read)();

typedef struct t_sensor_tristate {	
	uint8_t enable;

	uint16_t threshold_high;
	uint16_t threshold_low;
	uint32_t unit;
	uint8_t enable_counter;
	sensor_tristate_state_t raw_state;
	sensor_tristate_state_t pre_raw_state;

	uint8_t enable_filter;
	uint32_t filter_counter;		
	uint32_t low_state_filter_time;
	uint32_t medium_state_filter_time;
	uint32_t high_state_filter_time;
	sensor_tristate_state_t filter_state;

	uint8_t enable_hold;	
	uint32_t hold_counter;
	uint32_t low_state_hold_time;
	uint32_t medium_state_hold_time;
	uint32_t high_state_hold_time;
	sensor_tristate_state_t hold_state;

	int low_map_state;
	int medium_map_state;
	int high_map_state;

	pf_sensor_tristate_ctrl init;
	pf_sensor_tristate_read read;

} sensor_tristate_t;

extern sensor_tristate_status_t sensor_tristate_init(sensor_tristate_t* sensor_tristate, 
									uint32_t unit, 
									pf_sensor_tristate_ctrl init,
									pf_sensor_tristate_read read,
									uint16_t threshold_low, 
									uint16_t threshold_high);
extern sensor_tristate_status_t sensor_tristate_set_filter_time(sensor_tristate_t* sensor_tristate, 
										uint32_t low_time, 
										uint32_t medium_time, 
										uint32_t high_time);
extern sensor_tristate_status_t sensor_tristate_set_hold_time(sensor_tristate_t* sensor_tristate, 
										uint32_t low_time, 
										uint32_t medium_time, 
										uint32_t high_time);
extern void sensor_tristate_enable(sensor_tristate_t* sensor_tristate);
extern void sensor_tristate_disable(sensor_tristate_t* sensor_tristate);
extern void sensor_tristate_polling(sensor_tristate_t* sensor_tristate);
extern sensor_tristate_state_t sensor_tristate_read_raw(sensor_tristate_t* sensor_tristate);
extern sensor_tristate_state_t sensor_tristate_read_filter(sensor_tristate_t* sensor_tristate);
extern sensor_tristate_state_t sensor_tristate_read_hold(sensor_tristate_t* sensor_tristate);
extern void sensor_tristate_set_start_state(sensor_tristate_t* sensor_tristate, int state);
extern void sensor_tristate_set_map_state(sensor_tristate_t* sensor_tristate, int low_map, int medium_map, int high_map);
extern int sensor_tristate_read_raw_map(sensor_tristate_t* sensor_tristate);
extern int sensor_tristate_read_filter_map(sensor_tristate_t* sensor_tristate);
extern int sensor_tristate_read_hold_map(sensor_tristate_t* sensor_tristate);
#ifdef __cplusplus
}
#endif

#endif // __SENSOR_TRISTATE_H__
