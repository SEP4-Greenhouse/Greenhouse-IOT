#pragma once
#include <stdint.h>

void control_pir_init(void);
void control_pir_deinit(void);
uint8_t control_pir_is_motion_detected(void);