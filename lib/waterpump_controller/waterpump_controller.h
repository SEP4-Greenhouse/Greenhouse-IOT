#pragma once // Ensures this header is only included once during compilation

#include <stdint.h> // Includes standard integer types (not used directly here, but good practice for embedded headers)


/**
 * @brief Initializes the hardware used to control the water pump.
 * 
 * Typically sets the relevant microcontroller pin as output and sets it to OFF state.
 */
void control_waterpump_init(void);

/**
 * @brief Turns the water pump ON.
 * 
 * This sets the output pin high or active to start the pump.
 */
void control_waterpump_on(void);

/**
 * @brief Turns the water pump OFF.
 * 
 * This deactivates the output pin, stopping the pump.
 */
void control_waterpump_off(void);

/**
 * @brief Runs the water pump for exactly 5 seconds, then turns it off.
 * 
 * This is a convenience function for timed watering.
 */
void control_waterpump_run_5s(void);  // NEW function