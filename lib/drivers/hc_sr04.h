#ifndef HC_SR04_H
#define HC_SR04_H

#include <stdint.h>

/**
 * @brief Initializes the HC-SR04 proximity sensor.
 */
void hc_sr04_init(void);

/**
 * @brief Measures the distance to an object using HC-SR04.
 *
 * @return Distance in centimeters. Returns 0 on timeout/failure.
 */
uint16_t hc_sr04_takeMeasurement(void);

#endif // HC_SR04_H