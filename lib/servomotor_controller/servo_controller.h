#pragma once
#include <stdint.h>

void control_servo_init(void);
void control_servo_on(void);
void control_servo_off(void);
uint8_t control_servo_is_on(void);