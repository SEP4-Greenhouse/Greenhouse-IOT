#ifndef MOISTURE_CONTROLLER_H
#define MOISTURE_CONTROLLER_H

#include <stdint.h>

void control_moisture_init(void);
uint16_t control_moisture_get_raw_value(void);
uint8_t control_moisture_get_percent(void);
const char* control_moisture_get_level(uint16_t adc_value);

#endif