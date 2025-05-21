/**
 * @file hc_sr04.h
 * @brief Interface for HC-SR04 ultrasonic proximity sensor
 */

#ifndef HC_SR04_H
#define HC_SR04_H

#include <stdint.h>

/**
 * @brief Initializes the HC-SR04 sensor (trigger/echo pins).
 */
void hc_sr04_init(void);

/**
 * @brief Takes a distance measurement.
 * @return Distance in cm. Returns 0 on timeout/failure.
 */
uint16_t hc_sr04_takeMeasurement(void);

#endif // HC_SR04_H
