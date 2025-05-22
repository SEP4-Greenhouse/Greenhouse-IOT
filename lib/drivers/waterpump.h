/**
 * @file waterpump.h
 * @brief Water pump control interface for timed and manual operation.
 */

#ifndef WATERPUMP_H
#define WATERPUMP_H

#include <stdint.h>

// Configurable control pin (PC7 on PORTC)
#define PUMP_PORT PORTC
#define PUMP_DDR  DDRC
#define PUMP_PIN  PC7

/**
 * @brief Initializes the water pump hardware and Timer 4.
 * 
 * Configures the output pin and sets up Timer 4 to generate
 * 1 ms interval interrupts for managing timed pump activation.
 */
void pump_init(void);

/**
 * @brief Starts the pump for a specified time (non-blocking).
 * 
 * The pump will automatically turn off after the given duration.
 * A maximum runtime safety limit is enforced internally.
 * 
 * @param duration_ms Duration to run (in milliseconds)
 * @return 1 if started, 0 if already running or invalid duration
 */
uint8_t pump_run(uint32_t duration_ms);

/**
 * @brief Starts the pump indefinitely until manually stopped.
 * 
 * @return 1 if started, 0 if already running
 */
uint8_t pump_start(void);

/**
 * @brief Stops the pump immediately.
 */
void pump_stop(void);

/**
 * @brief Checks if the pump is currently running.
 * 
 * @return 1 if running, 0 otherwise
 */
uint8_t pump_is_running(void);

#endif // WATERPUMP_H