#include <stdint.h> 
#include "waterpump.h"

// Simulated AVR IO registers
uint8_t PORTL = 0;
uint8_t DDRL = 0;
#define PL6 6

void waterpump_init(void) {
    DDRL |= (1 << PL6);     // Set PL6 as output
    PORTL &= ~(1 << PL6);   // Ensure pump is OFF
}

void waterpump_start(void) {
    PORTL |= (1 << PL6);    // Simulate turning pump ON
}

void waterpump_stop(void) {
    PORTL &= ~(1 << PL6);   // Simulate turning pump OFF
}
