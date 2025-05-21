/**
 * @file pir.h
 * @brief Interface for PIR motion sensor driver
 */

#ifndef PIR_H
#define PIR_H

#include <stdint.h>

typedef void (*pir_callback_t)(void);

/**
 * @brief Initializes PIR sensor and interrupt on motion detection.
 * @param callback Function to call on motion detection.
 */
void pir_init(pir_callback_t callback);

/**
 * @brief Deinitializes PIR sensor and disables interrupt.
 */
void pir_deinit(void);

/**
 * @brief Polls current motion state (optional use).
 * @return 1 if motion is detected, 0 otherwise.
 */
uint8_t pir_is_motion_detected(void);

#endif // PIR_H