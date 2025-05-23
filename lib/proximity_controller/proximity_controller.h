#pragma once
#include <stdint.h>

void control_proximity_init(void);
uint16_t control_proximity_get_distance_cm(void);
uint8_t control_proximity_is_close(uint16_t threshold_cm);