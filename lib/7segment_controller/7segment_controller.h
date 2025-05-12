#pragma once
#include <stdint.h>

void control_display_init(void);
void control_display_set_number(int16_t number);
void control_display_custom_segments(uint8_t seg1, uint8_t seg2, uint8_t seg3, uint8_t seg4);
