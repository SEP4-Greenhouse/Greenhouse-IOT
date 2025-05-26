#pragma once
#include <stdint.h>

void control_servo_init(void);
void control_servo_step_clockwise(void);
void control_servo_step_counterclockwise(void);
void control_servo_set_angle(uint8_t angle);
uint8_t control_servo_get_angle(void);
const char* control_servo_get_position_status(void);