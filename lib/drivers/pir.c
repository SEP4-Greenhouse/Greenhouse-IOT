/**
 * @file pir.c
 * @brief PIR motion sensor driver for ATmega2560 using external interrupt INT2
 *
 * Detects motion via a PIR sensor on PD2 (INT2). When motion is detected,
 * a user-defined callback is executed inside the interrupt handler.
 */

#include "pir.h"
#include "includes.h"

// === PIR Pin Configuration ===
#define PIR_DDR     DDRD
#define PIR_PORT    PORTD
#define PIR_PIN     PIND
#define PIR_BIT     PD2  // Connected to INT2

// === Internal Motion Callback Pointer ===
static pir_callback_t pir_callback = NULL;

#ifndef WINDOWS_TEST
/**
 * @brief ISR for INT2 triggered on rising edge (motion detected).
 */
ISR(INT2_vect) {
    if (pir_callback) {
        pir_callback();
    }
}
#endif

/**
 * @brief Initializes the PIR sensor and sets up interrupt on INT2.
 *
 * @param callback Function to call on motion detection.
 */
void pir_init(pir_callback_t callback) {
    // Set PIR pin as input with pull-up resistor
    PIR_DDR &= ~(1 << PIR_BIT);
    PIR_PORT |= (1 << PIR_BIT);

    // Configure INT2 for rising edge trigger (motion detected)
    EICRA = (EICRA & ~((1 << ISC21) | (1 << ISC20))) | ((1 << ISC21) | (1 << ISC20));
    EIMSK |= (1 << INT2);  // Enable INT2 interrupt

    // Store user callback
    pir_callback = callback;

    // Enable global interrupts
    sei();
}

/**
 * @brief Disables PIR interrupt and clears callback.
 */
void pir_deinit(void) {
    EIMSK &= ~(1 << INT2);  // Disable INT2 interrupt
    pir_callback = NULL;
}

/**
 * @brief Optional polling method to check current PIR signal state.
 * 
 * @return 1 if motion is currently detected, 0 otherwise.
 */
uint8_t pir_is_motion_detected(void) {
    return (PIR_PIN & (1 << PIR_BIT)) ? 1 : 0;
}