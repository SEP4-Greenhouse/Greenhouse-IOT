/**
 * @file pir.c
 * @brief PIR motion sensor driver using external interrupt INT2
 */

#include "pir.h"
#include "includes.h"

#define PIR_DDR  DDRD
#define PIR_PORT PORTD
#define PIR_PIN  PIND
#define PIR_BIT  PD2

// Function pointer for motion detection callback
static pir_callback_t pir_callback = NULL;

#ifndef WINDOWS_TEST
// Interrupt Service Routine triggered when motion is detected
ISR(INT2_vect) {
    if (pir_callback) {
        pir_callback();
    }
}
#endif

/**
 * @brief Initialize PIR sensor and register interrupt callback
 *
 * Sets up INT2 external interrupt on pin PD2
 */
void pir_init(pir_callback_t callback) {
    PIR_DDR &= ~(1 << PIR_BIT);  // Set PD2 as input
    PIR_PORT |= (1 << PIR_BIT);  // Enable pull-up resistor

    // Configure interrupt: falling edge on INT2 (motion event)
    EICRA = (EICRA & ~((1 << ISC21) | (1 << ISC20))) | ((1 << ISC21) | (1 << ISC20));
    EIMSK |= (1 << INT2);       // Enable external interrupt INT2

    pir_callback = callback;    // Register user-defined callback

    sei();
}

// Disables the PIR sensor's interrupt
void pir_deinit(void) {
    EIMSK &= ~(1 << INT2);  // Disable INT2 interrupt
    pir_callback = NULL;    // Remove callback
}

// Reads the PIR input pin directly to check for motion (logic HIGH)
uint8_t pir_is_motion_detected(void) {
    return (PIR_PIN & (1 << PIR_BIT)) ? 1 : 0;
}