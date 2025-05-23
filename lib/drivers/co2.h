/**
 * @file co2.h
 * @brief Interface for CO2 sensor (e.g., MH-Z19) UART driver
 */

#ifndef CO2_H
#define CO2_H

#include <stdint.h>
#include <stdbool.h>  // For bool type

/**
 * @brief Initializes the UART for communication with the CO2 sensor.
 */
void co2_init(void);

/**
 * @brief Reads the current CO2 concentration in ppm.
 * 
 * @return 16-bit CO2 value in ppm (0 if error)
 */
uint16_t co2_read_ppm(void);

/**
 * @brief Enables or disables automatic baseline calibration.
 *
 * @param enable Pass true to enable calibration, false to disable.
 */
void co2_set_auto_calibration(bool enable);

#endif // CO2_H