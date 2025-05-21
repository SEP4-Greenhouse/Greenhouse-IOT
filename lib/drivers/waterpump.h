// #pragma once
// #include <stdint.h>

// void waterpump_init(void);
// void waterpump_start(void);
// void waterpump_stop(void);


#pragma once

#include <stdint.h>

// Pin used to control the water pump (connected to PC7 / OC4A)
#define PUMP_PIN PC7

/**
 * @brief Initializes the water pump system.
 *
 * Configures the control pin and sets up Timer 4 for 1 ms interrupts.
 */
void pump_init(void);

/**
 * @brief Starts the pump for a specific duration.
 *
 * Turns on the pump and schedules it to stop automatically after the specified time.
 *
 * @param duration_ms Duration in milliseconds to run the pump.
 * @return 1 if the pump was started, 0 if it's already running.
 */
uint8_t pump_run(uint32_t duration_ms);

/**
 * @brief Starts the pump indefinitely.
 *
 * @return 1 if the pump was started, 0 if it's already running.
 */
uint8_t pump_start(void);

/**
 * @brief Stops the pump immediately.
 */
void pump_stop(void);

/**
 * @brief Checks if the pump is currently running.
 *
 * @return 1 if running, 0 otherwise.
 */
uint8_t pump_is_running(void);