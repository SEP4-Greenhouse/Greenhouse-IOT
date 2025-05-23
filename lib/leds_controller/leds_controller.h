#pragma once
#include <stdint.h>

void control_leds_init(void);
void control_led_on(uint8_t led_no);
void control_led_off(uint8_t led_no);
void control_led_toggle(uint8_t led_no);
