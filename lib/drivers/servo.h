#pragma once
#include <stdint.h>

void servo_init(void);
void servo(uint8_t angle);
void servo_set_angle(uint8_t angle);
uint8_t servo_get_angle(void);