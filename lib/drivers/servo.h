#pragma once
#include <stdint.h>

void servo_init(void);
void servo_on(void);
void servo_off(void);
uint8_t servo_is_on(void);