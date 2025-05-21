/**
 * @file co2_sensor.h
 * @brief Interface for CO2 sensor (e.g., MH-Z19 via UART)
 */

#ifndef CO2_H
#define CO2_H

#include <stdint.h>

/**
 * @brief Initializes UART communication for CO2 sensor.
 */
void co2_init(void);

/**
 * @brief Reads CO₂ ppm from sensor.
 * @return CO₂ concentration in ppm (0 on error).
 */
uint16_t co2_read_ppm(void);

#endif // CO2_H