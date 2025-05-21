/**
 * @file light.h
 * @brief Interface for the photoresistor (LDR) sensor driver
 *
 * Provides initialization and analog value reading for a photoresistor
 * connected to ADC15 (PK7) on the ATmega2560.
 */

#ifndef LIGHT_H
#define LIGHT_H

#include <avr/io.h>
#include <stdint.h>

/**
 * @brief Initializes the ADC to read the light sensor on ADC15 (PK7).
 */
void light_init(void);

/**
 * @brief Reads the current light level from the photoresistor.
 *
 * @return 10-bit ADC value (0–1023), or 0 on error/timeout
 */
uint16_t light_read(void);

/**
 * @brief Disables the ADC to reduce power consumption.
 */
void light_deinit(void);

#endif // LIGHT_H