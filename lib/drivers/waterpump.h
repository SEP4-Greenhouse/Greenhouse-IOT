// #pragma once
// #include <stdint.h>

// void waterpump_init(void);
// void waterpump_start(void);
// void waterpump_stop(void);

#pragma once

#include <stdint.h>

#define PUMP_PIN PC7

void pump_init(void);
uint8_t pump_run(uint32_t duration_ms);
uint8_t pump_start(void);
void pump_stop(void);
uint8_t pump_is_running(void);