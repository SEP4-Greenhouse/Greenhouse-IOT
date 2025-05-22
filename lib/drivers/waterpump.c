/**
 * @file waterpump.c
 * @brief Implementation of water pump control using Timer 4 (ATmega2560).
 */

#include "waterpump.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Optional: max allowed runtime (safety cap)
#define MAX_PUMP_DURATION_MS 10000UL  // 10 seconds max

// Internal state
volatile uint32_t pump_duration = 0;
volatile uint8_t pump_running = 0;

/**
 * @brief (Optional) Stub to simulate MQTT or serial status publishing.
 */
static void send_pump_status(const char* topic, const char* msg) {
    // Replace this with mqtt_publish(topic, msg) or uart_send_string
}

/**
 * @brief Initialize water pump control and Timer 4 (1ms interval).
 */
void pump_init(void) {
    PUMP_DDR |= (1 << PUMP_PIN);
    PUMP_PORT &= ~(1 << PUMP_PIN);  // Ensure OFF

    // Configure Timer 4 for 1ms CTC interrupts
    TCCR4A = 0;
    TCCR4B = 0;
    TCNT4 = 0;

    TCCR4B |= (1 << WGM42);                   // CTC mode
    TCCR4B |= (1 << CS41) | (1 << CS40);      // Prescaler = 64
    OCR4A = 249;                              // 16MHz/64/250 = 1kHz = 1ms

    TIMSK4 |= (1 << OCIE4A);                  // Enable compare match interrupt
    sei();                                    // Global interrupt enable
}

uint8_t pump_run(uint32_t duration_ms) {
    if (duration_ms == 0 || duration_ms > MAX_PUMP_DURATION_MS) return 0;

    cli(); // critical section
    if (pump_running) {
        sei();
        return 0;
    }

    pump_duration = duration_ms;
    pump_running = 1;
    PUMP_PORT |= (1 << PUMP_PIN);
    sei();

    send_pump_status("pump/started", "Pump started");
    return 1;
}

uint8_t pump_start(void) {
    cli();
    if (pump_running) {
        sei();
        return 0;
    }

    pump_running = 1;
    pump_duration = 0;
    PUMP_PORT |= (1 << PUMP_PIN);
    sei();

    send_pump_status("pump/started", "Pump started (manual)");
    return 1;
}

void pump_stop(void) {
    cli();
    PUMP_PORT &= ~(1 << PUMP_PIN);
    pump_duration = 0;
    pump_running = 0;
    sei();

    send_pump_status("pump/stopped", "Pump stopped");
}

uint8_t pump_is_running(void) {
    return pump_running;
}

ISR(TIMER4_COMPA_vect) {
    if (pump_running && pump_duration > 0) {
        pump_duration--;
        if (pump_duration == 0) {
            PUMP_PORT &= ~(1 << PUMP_PIN);
            pump_running = 0;
            send_pump_status("pump/stopped", "Pump stopped (timeout)");
        }
    }
}