#pragma once
#include <stdint.h>
#include <stdbool.h>

void control_pir_init(void);
void control_pir_deinit(void);
uint8_t control_pir_is_motion_detected(void);
bool control_pir_detected(void);