/**
 * @file hc_sr04.c
 * @brief Ultrasonic proximity sensor driver (HC-SR04) for ATmega2560
 */

#include "hc_sr04.h"
#include "includes.h"
#include <util/delay.h>
#include <avr/interrupt.h>

// Pin configuration
#define TRIGGER_DDR     DDRL
#define TRIGGER_PORT    PORTL
#define TRIGGER_PIN     PL7

#define ECHO_DDR        DDRL
#define ECHO_PIN_REG    PINL
#define ECHO_PIN        PL6

// Timing constants
#define TIMER_PRESCALER     256
#define MAX_WAIT_START_US   100000UL
#define MAX_WAIT_ECHO_US    24000UL
#define SOUND_SPEED_DIVISOR 125UL

// Initializes the HC-SR04 sensor's pins
void hc_sr04_init(void) {
    TRIGGER_DDR |= (1 << TRIGGER_PIN);      // Set trigger pin (PL7) as output
    TRIGGER_PORT &= ~(1 << TRIGGER_PIN);    // Ensure trigger is LOW initially
    ECHO_DDR &= ~(1 << ECHO_PIN);           // Set echo pin (PL6) as input
}

// Measures distance based on ultrasonic echo timing
uint16_t hc_sr04_takeMeasurement(void) {
    uint16_t count = 0;
    uint8_t saved_TCCR1B = TCCR1B;

    cli();
    TCCR1B = (1 << CS12); // Timer1 with prescaler 256
    TCNT1 = 0;

    TRIGGER_PORT &= ~(1 << TRIGGER_PIN);
    _delay_us(2);
    TRIGGER_PORT |= (1 << TRIGGER_PIN);
    _delay_us(10);
    TRIGGER_PORT &= ~(1 << TRIGGER_PIN);

    while (!(ECHO_PIN_REG & (1 << ECHO_PIN))) {
        if (TCNT1 >= (F_CPU / TIMER_PRESCALER) * (MAX_WAIT_START_US / 1e6)) {
            TCCR1B = saved_TCCR1B;
            sei();
            return 0;
        }
    }

    TCNT1 = 0;
    while (ECHO_PIN_REG & (1 << ECHO_PIN)) {
        if (TCNT1 >= (F_CPU / TIMER_PRESCALER) * (MAX_WAIT_ECHO_US / 1e6)) {
            TCCR1B = saved_TCCR1B;
            sei();
            return 0;
        }
    }

    count = TCNT1;
    TCCR1B = saved_TCCR1B;
    sei();

    // gives one-way distance in cm
    return (uint16_t)((count * 549UL) / 1000);  // returns one-way distance in cm
}

// High-level API to get distance in cm
uint16_t hc_sr04_getDistance(void) {
    uint16_t distance = hc_sr04_takeMeasurement();
    if (distance == 0) {
        return 0;
    }
    return distance;
}