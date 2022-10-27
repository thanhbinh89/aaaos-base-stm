#include "sensor_tristate.h"

#define SENSOR_TRISTATE_DISABLE	0x00
#define SENSOR_TRISTATE_ENABLE	0x01

sensor_tristate_status_t sensor_tristate_init(sensor_tristate_t* sensor_tristate, 
							uint32_t unit, 
							pf_sensor_tristate_ctrl init,
							pf_sensor_tristate_read read,
							uint16_t threshold_low, 
							uint16_t threshold_high) {
	sensor_tristate->enable = SENSOR_TRISTATE_DISABLE;	
	
	sensor_tristate->threshold_low = threshold_low;
	sensor_tristate->threshold_high = threshold_high;
	sensor_tristate->unit = unit;
	sensor_tristate->enable_counter = SENSOR_TRISTATE_DISABLE;
	sensor_tristate->raw_state = SENSOR_TRISTATE_STATE_UNKNOWN;
	sensor_tristate->pre_raw_state = SENSOR_TRISTATE_STATE_UNKNOWN;

	sensor_tristate->enable_filter = SENSOR_TRISTATE_DISABLE;
	sensor_tristate->enable_hold = SENSOR_TRISTATE_DISABLE;
	
	sensor_tristate->filter_state = SENSOR_TRISTATE_STATE_UNKNOWN;
	sensor_tristate->hold_state = SENSOR_TRISTATE_STATE_UNKNOWN;

	sensor_tristate->low_map_state = SENSOR_TRISTATE_STATE_LOW;
	sensor_tristate->medium_map_state = SENSOR_TRISTATE_STATE_MEDIUM;
	sensor_tristate->high_map_state = SENSOR_TRISTATE_STATE_HIGH;

	sensor_tristate->init = init;
	sensor_tristate->read = read;

	if (sensor_tristate->init) {
		sensor_tristate->init();
	}
	else {
		return SENSOR_TRISTATE_STATUS_FAIL;
	}

	if (!sensor_tristate->read) {
		return SENSOR_TRISTATE_STATUS_FAIL;
	}

	return SENSOR_TRISTATE_STATUS_SUCCESS;
}

sensor_tristate_status_t sensor_tristate_set_filter_time(sensor_tristate_t* sensor_tristate, 
								uint32_t low_time, 
								uint32_t medium_time, 
								uint32_t high_time) {
	sensor_tristate->low_state_filter_time = low_time;
	sensor_tristate->medium_state_filter_time = medium_time;
	sensor_tristate->high_state_filter_time = high_time;
	sensor_tristate->filter_counter = 0;
	sensor_tristate->enable_filter = SENSOR_TRISTATE_ENABLE;
	return SENSOR_TRISTATE_STATUS_SUCCESS;
}

sensor_tristate_status_t sensor_tristate_set_hold_time(sensor_tristate_t* sensor_tristate, 
										uint32_t low_time, 
										uint32_t medium_time, 
										uint32_t high_time) {
	if (sensor_tristate->enable_filter == SENSOR_TRISTATE_ENABLE) {
		sensor_tristate->low_state_hold_time = low_time;
		sensor_tristate->medium_state_hold_time = medium_time;
		sensor_tristate->high_state_hold_time = high_time;
		sensor_tristate->hold_counter = 0;
		sensor_tristate->enable_hold = SENSOR_TRISTATE_ENABLE;
		return SENSOR_TRISTATE_STATUS_SUCCESS;
	}
	return SENSOR_TRISTATE_STATUS_FAIL;
}

void sensor_tristate_enable(sensor_tristate_t* sensor_tristate) {
	sensor_tristate->enable = SENSOR_TRISTATE_ENABLE;
}

void sensor_tristate_disable(sensor_tristate_t* sensor_tristate) {
	sensor_tristate->enable = SENSOR_TRISTATE_DISABLE;
}

void sensor_tristate_polling(sensor_tristate_t* sensor_tristate) {
	uint16_t adc;
	sensor_tristate_state_t new_hold_state;

	if (sensor_tristate->enable == SENSOR_TRISTATE_ENABLE) {

		/* convert ADC value to tristate */
		adc = sensor_tristate->read();
		if (adc >= sensor_tristate->threshold_high) sensor_tristate->raw_state = SENSOR_TRISTATE_STATE_HIGH;
		else if (adc >= sensor_tristate->threshold_low) sensor_tristate->raw_state = SENSOR_TRISTATE_STATE_MEDIUM;
		else sensor_tristate->raw_state = SENSOR_TRISTATE_STATE_LOW;

		/* set filter_state first time */
		/* comment because noise first time */
		// if (sensor_tristate->filter_state == SENSOR_TRISTATE_STATE_UNKNOWN) sensor_tristate->filter_state = sensor_tristate->raw_state;
		/* set hold_state first time */
		// if (sensor_tristate->hold_state == SENSOR_TRISTATE_STATE_UNKNOWN) sensor_tristate->hold_state = sensor_tristate->raw_state;
		
		/* filter state */
		if (sensor_tristate->enable_filter == SENSOR_TRISTATE_ENABLE) {
			
			if (sensor_tristate->raw_state != sensor_tristate->pre_raw_state) {
				sensor_tristate->pre_raw_state = sensor_tristate->raw_state;
				sensor_tristate->filter_counter = 0;
				sensor_tristate->enable_counter = SENSOR_TRISTATE_ENABLE;
			}
			else if (sensor_tristate->enable_counter == SENSOR_TRISTATE_ENABLE) {
				sensor_tristate->filter_counter += sensor_tristate->unit;

				switch (sensor_tristate->raw_state)
				{
				case SENSOR_TRISTATE_STATE_LOW:
					if (sensor_tristate->filter_counter >= sensor_tristate->low_state_filter_time) {
						sensor_tristate->enable_counter = SENSOR_TRISTATE_DISABLE;
						sensor_tristate->filter_state = sensor_tristate->raw_state;
					}
					break;

				case SENSOR_TRISTATE_STATE_MEDIUM:
					if (sensor_tristate->filter_counter >= sensor_tristate->medium_state_filter_time) {
						sensor_tristate->enable_counter = SENSOR_TRISTATE_DISABLE;
						sensor_tristate->filter_state = sensor_tristate->raw_state;
					}
					break;

				case SENSOR_TRISTATE_STATE_HIGH:
					if (sensor_tristate->filter_counter >= sensor_tristate->high_state_filter_time) {
						sensor_tristate->enable_counter = SENSOR_TRISTATE_DISABLE;
						sensor_tristate->filter_state = sensor_tristate->raw_state;
					}
					break;
				
				default:
					break;
				}				
			}

			/* hold state */
			if (sensor_tristate->enable_hold == SENSOR_TRISTATE_ENABLE) {
				new_hold_state = sensor_tristate->filter_state;

				if (sensor_tristate->hold_state == new_hold_state) {					
					sensor_tristate->hold_counter = 0;
				}
				else {
					sensor_tristate->hold_counter += sensor_tristate->unit;

					switch (sensor_tristate->hold_state)
					{
					case SENSOR_TRISTATE_STATE_LOW:
						if (sensor_tristate->hold_counter >= sensor_tristate->low_state_hold_time) {							
							sensor_tristate->hold_state = new_hold_state;
							sensor_tristate->hold_counter = 0;
						}
						break;

					case SENSOR_TRISTATE_STATE_MEDIUM:
						if (sensor_tristate->hold_counter >= sensor_tristate->medium_state_hold_time) {
							sensor_tristate->hold_state = new_hold_state;
							sensor_tristate->hold_counter = 0;
						}
						break;

					case SENSOR_TRISTATE_STATE_HIGH:
						if (sensor_tristate->hold_counter >= sensor_tristate->high_state_hold_time) {
							sensor_tristate->hold_state = new_hold_state;
							sensor_tristate->hold_counter = 0;
						}
						break;

					case SENSOR_TRISTATE_STATE_UNKNOWN:
						sensor_tristate->hold_state = new_hold_state;
						sensor_tristate->hold_counter = 0;
						break;
					
					default:
						break;
					}
				}
			}
		}
	}
}

sensor_tristate_state_t sensor_tristate_read_raw(sensor_tristate_t* sensor_tristate) {
	return sensor_tristate->raw_state;
}

sensor_tristate_state_t sensor_tristate_read_filter(sensor_tristate_t* sensor_tristate) {
	return sensor_tristate->filter_state;
}

sensor_tristate_state_t sensor_tristate_read_hold(sensor_tristate_t* sensor_tristate) {
	return sensor_tristate->hold_state;
}

void sensor_tristate_set_start_state(sensor_tristate_t* sensor_tristate, int state) {
	sensor_tristate->filter_state = state;
	sensor_tristate->hold_state = state;
}

void sensor_tristate_set_map_state(sensor_tristate_t* sensor_tristate, int low_map, int medium_map, int high_map) {
	sensor_tristate->low_map_state = low_map;
	sensor_tristate->medium_map_state = medium_map;
	sensor_tristate->high_map_state = high_map;
}

int sensor_tristate_read_raw_map(sensor_tristate_t* sensor_tristate) {
	switch (sensor_tristate->raw_state)
	{
	case SENSOR_TRISTATE_STATE_LOW:
		return sensor_tristate->low_map_state;
		break;

	case SENSOR_TRISTATE_STATE_MEDIUM:
		return sensor_tristate->medium_map_state;
		break;

	case SENSOR_TRISTATE_STATE_HIGH:
		return sensor_tristate->high_map_state;
		break;
	
	default:
		break;
	}
	return sensor_tristate->raw_state;
}

int sensor_tristate_read_filter_map(sensor_tristate_t* sensor_tristate) {
	switch (sensor_tristate->filter_state)
	{
	case SENSOR_TRISTATE_STATE_LOW:
		return sensor_tristate->low_map_state;
		break;

	case SENSOR_TRISTATE_STATE_MEDIUM:
		return sensor_tristate->medium_map_state;
		break;

	case SENSOR_TRISTATE_STATE_HIGH:
		return sensor_tristate->high_map_state;
		break;
	
	default:
		break;
	}
	return sensor_tristate->filter_state;
}

int sensor_tristate_read_hold_map(sensor_tristate_t* sensor_tristate) {
	switch (sensor_tristate->hold_state)
	{
	case SENSOR_TRISTATE_STATE_LOW:
		return sensor_tristate->low_map_state;
		break;

	case SENSOR_TRISTATE_STATE_MEDIUM:
		return sensor_tristate->medium_map_state;
		break;

	case SENSOR_TRISTATE_STATE_HIGH:
		return sensor_tristate->high_map_state;
		break;
	
	default:
		break;
	}
	return sensor_tristate->hold_state;
}
