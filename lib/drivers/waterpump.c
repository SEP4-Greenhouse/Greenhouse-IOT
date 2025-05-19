// #include <stdint.h>
// #include "waterpump.h"

// // Simulated AVR IO registers for unit testing only
// uint8_t PORTL = 0;
// uint8_t DDRL = 0;
// #define PL6 6

// void waterpump_init(void)
// {
//     DDRL |= (1 << PL6);       // Set PL6 as output
//     PORTL &= ~(1 << PL6);     // Ensure pump is off
// }

// void waterpump_start(void)
// {
//     PORTL |= (1 << PL6);      // Turn on pump (PL6 high)
// }

// void waterpump_stop(void)
// {
//     PORTL &= ~(1 << PL6);     // Turn off pump (PL6 low)
// }

#include "waterpump.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define PUMP_DDR DDRC
#define PUMP_PORT PORTC

// Global variables
volatile uint32_t pump_duration = 0;
volatile uint8_t pump_running = 0;

void pump_init(void) {
    PUMP_DDR |= (1 << PUMP_PIN);    // Set pump pin as output
    PUMP_PORT &= ~(1 << PUMP_PIN);  // Ensure pump is off

    // Configure Timer4 for 1ms interrupts using CTC mode
    TCCR4A = 0;
    TCCR4B = (1 << WGM42) | (1 << CS41); // CTC mode, prescaler 8
    OCR4A = 1999;  // Assuming 16 MHz clock => 1ms = (16*10^6)/(8*(1999+1))
    TIMSK4 = (1 << OCIE4A); // Enable compare match interrupt
}

uint8_t pump_start(void) {
    if (pump_running) return 0;
    pump_running = 1;
    PUMP_PORT |= (1 << PUMP_PIN);
    return 1;
}

uint8_t pump_run(uint32_t duration_ms) {
    if (pump_running) return 0;
    pump_duration = duration_ms;
    pump_running = 1;
    PUMP_PORT |= (1 << PUMP_PIN);
    return 1;
}

void pump_stop(void) {
    pump_running = 0;
    pump_duration = 0;
    PUMP_PORT &= ~(1 << PUMP_PIN);
}

uint8_t pump_is_running(void) {
    return pump_running;
}

ISR(TIMER4_COMPA_vect) {
    if (pump_running && pump_duration > 0) {
        pump_duration--;
        if (pump_duration == 0) {
            pump_stop();
        }
    }
}