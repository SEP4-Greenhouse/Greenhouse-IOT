#pragma once
#include <stdint.h>

void control_servo_init(void);
void control_servo_set_angle(uint8_t angle);
void control_servo_sweep(void);