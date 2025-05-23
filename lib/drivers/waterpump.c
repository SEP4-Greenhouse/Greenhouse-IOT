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

#ifdef __AVR__

#include "waterpump.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// PC7 (J8-2) is used to control the pump via Timer 4
#define PUMP_PIN PC7

// Pump status and timing
volatile uint32_t pump_duration = 0;  // Countdown timer in milliseconds
volatile uint8_t pump_running = 0;    // 1 = running, 0 = stopped






// Temporary stub to fix undefined reference error
void send_pump_status(const char* topic, const char* message) {
    // Do nothing (or print to console if needed)
}





/**
 * Initializes the water pump hardware and configures Timer 4
 * for 1ms interval interrupts to control timed pump activation.
 */
void pump_init(void) {
    // Configure PC7 as output (pump control pin)
    DDRC |= (1 << PUMP_PIN);
    PORTC &= ~(1 << PUMP_PIN);  // Ensure pump is OFF initially

    // Reset Timer 4
    TCCR4A = 0;
    TCCR4B = 0;
    TCNT4 = 0;

    // Enable CTC mode (Clear Timer on Compare Match)
    TCCR4B |= (1 << WGM42);

    // Prescaler = 64 => 16MHz / 64 = 250kHz
    // OCR4A = 249 => 250kHz / (249 + 1) = 1kHz = 1ms
    TCCR4B |= (1 << CS41) | (1 << CS40);
    OCR4A = 249;

    // Enable Compare Match A interrupt for Timer 4
    TIMSK4 |= (1 << OCIE4A);

    sei();  // Enable global interrupts
}

/**
 * Turns on the pump for a given duration (in milliseconds).
 * Returns 1 if the operation started, or 0 if the pump is already running.
 */
uint8_t pump_run(uint32_t duration_ms) {
    cli();  // Atomic access to shared state

    if (pump_running) {
        sei();
        return 0;  // Already running
    }

    pump_duration = duration_ms;
    pump_running = 1;
    PORTC |= (1 << PUMP_PIN);  // Activate pump
    send_pump_status("pump/started", "pump started");

    sei();
    return 1;
}

/**
 * Immediately stops the pump and resets its state.
 */
void pump_stop(void) {
    cli();

    PORTC &= ~(1 << PUMP_PIN);  // Deactivate pump
    pump_duration = 0;
    pump_running = 0;

    sei();
}

/**
 * Checks if the pump is currently active.
 * @return 1 if running, 0 otherwise.
 */
uint8_t pump_is_running(void) {
    return pump_running;
}

/**
 * Starts the pump without setting a duration (runs indefinitely).
 * Returns 1 if started, or 0 if it was already running.
 */
uint8_t pump_start(void) {
    if (pump_running) {
        return 0;
    }

    pump_running = 1;
    PORTC |= (1 << PUMP_PIN);
    send_pump_status("pump/started", "pump started");

    return 1;
}

/**
 * Timer 4 Compare Match A interrupt service routine.
 * Called every 1ms to count down the active duration.
 * Stops the pump automatically when the timer expires.
 */
ISR(TIMER4_COMPA_vect) {
    if (pump_running && pump_duration > 0) {
        pump_duration--;

        if (pump_duration == 0) {
            PORTC &= ~(1 << PUMP_PIN);  // Deactivate pump
            pump_running = 0;
            send_pump_status("pump/stopped", "pump stopped");
        }
    }
}

#endif  // __AVR__