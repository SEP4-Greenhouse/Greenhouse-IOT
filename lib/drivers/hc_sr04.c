/**
 * @file hc_sr04.c
 * @brief Ultrasonic proximity sensor driver (HC-SR04) for ATmega2560
 *
 * Measures distance using an HC-SR04 sensor connected to:
 *  - Trigger: PL7 (digital output)
 *  - Echo: PL6 (digital input, pulse width measurement)
 */

#include "includes.h"
#include <util/delay.h>
#include <avr/interrupt.h>  // For cli()/sei()

// === Pin Configuration ===
#define TRIGGER_DDR     DDRL
#define TRIGGER_PORT    PORTL
#define TRIGGER_PIN     PL7

#define ECHO_DDR        DDRL
#define ECHO_PIN_REG    PINL
#define ECHO_PIN        PL6

// === Timing Constants ===
#define TIMER_PRESCALER         256
#define MAX_WAIT_START_US       100000UL  // 100ms
#define MAX_WAIT_ECHO_US        24000UL   // 24ms (max distance: ~4 meters)
#define SOUND_SPEED_DIVISOR     125UL     // (F_CPU * PRESCALER / 2 / 34300)

// === Initialization Function ===
void hc_sr04_init(void) {
    // Set Trigger as output
    TRIGGER_DDR |= (1 << TRIGGER_PIN);
    TRIGGER_PORT &= ~(1 << TRIGGER_PIN);  // Ensure low

    // Set Echo as input
    ECHO_DDR &= ~(1 << ECHO_PIN);
}

/**
 * @brief Measure distance using HC-SR04 sensor.
 *
 * @return Distance in centimeters. Returns 0 on timeout or failure.
 */
uint16_t hc_sr04_takeMeasurement(void) {
    uint16_t count = 0;

    // Send 10us trigger pulse
    TRIGGER_PORT &= ~(1 << TRIGGER_PIN);
    _delay_us(2);
    TRIGGER_PORT |= (1 << TRIGGER_PIN);
    _delay_us(10);
    TRIGGER_PORT &= ~(1 << TRIGGER_PIN);

    // Backup current Timer1 config
    uint8_t saved_TCCR1B = TCCR1B;

    // Disable interrupts during timing (optional but recommended for accuracy)
    cli();

    // Configure Timer1: Normal mode, prescaler 256
    TCCR1B = (1 << CS12);
    TCNT1 = 0;

    // Wait for echo pin to go HIGH
    while (!(ECHO_PIN_REG & (1 << ECHO_PIN))) {
        if (TCNT1 >= (F_CPU / TIMER_PRESCALER) * (MAX_WAIT_START_US / 1000000.0)) {
            TCCR1B = saved_TCCR1B;
            sei();
            return 0;  // Timeout waiting for echo start
        }
    }

    // Start timing pulse width
    TCNT1 = 0;
    while (ECHO_PIN_REG & (1 << ECHO_PIN)) {
        if (TCNT1 >= (F_CPU / TIMER_PRESCALER) * (MAX_WAIT_ECHO_US / 1000000.0)) {
            TCCR1B = saved_TCCR1B;
            sei();
            return 0;  // Timeout during echo HIGH
        }
    }

    count = TCNT1;

    // Restore previous Timer1 config and re-enable interrupts
    TCCR1B = saved_TCCR1B;
    sei();

    // Calculate and return distance (in cm)
    return (uint16_t)((count * 343UL) / SOUND_SPEED_DIVISOR);
}