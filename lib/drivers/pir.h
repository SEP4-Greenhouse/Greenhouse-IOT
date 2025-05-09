#ifndef PIR_H
#define PIR_H

#include <stdint.h>

/**
 * @brief Type for the PIR motion detection callback function.
 */
typedef void (*pir_callback_t)(void);

/**
 * @brief Initializes the PIR sensor with an interrupt-based callback.
 *
 * @param callback Function to call when motion is detected.
 */
void pir_init(pir_callback_t callback);

/**
 * @brief Deinitializes the PIR sensor (disables interrupt).
 */
void pir_deinit(void);

/**
 * @brief Checks the current state of the PIR signal.
 *
 * @return 1 if motion is currently detected, 0 otherwise.
 */
uint8_t pir_is_motion_detected(void);

#endif // PIR_H