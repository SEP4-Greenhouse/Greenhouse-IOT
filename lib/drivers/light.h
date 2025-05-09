#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>

/**
 * @brief Initializes the ADC hardware for the photoresistor.
 */
void light_init(void);

/**
 * @brief Reads the current light level from the photoresistor.
 *
 * @return 10-bit ADC value (0–1023). Returns 0 on timeout.
 */
uint16_t light_read(void);

#endif // LIGHT_H