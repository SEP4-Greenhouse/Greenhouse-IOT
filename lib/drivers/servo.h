#pragma once
#include <stdint.h>

void servo_init(void);
void servo(uint8_t angle);  // Set angle between 0 and 180