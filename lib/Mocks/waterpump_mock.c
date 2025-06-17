#include <stdint.h> 
#include "waterpump.h"  // Header containing the declarations of water pump functions

// ------------------- Simulated AVR I/O Registers -------------------

// Simulated PORTL register (output register for Port L)
uint8_t PORTL = 0;

// Simulated DDRL register (data direction register for Port L)
uint8_t DDRL = 0;

// Define bit position for PL6 (the pin connected to the water pump)
#define PL6 6

// ------------------------ Water Pump API ---------------------------

// Initializes the water pump by configuring the control pin (PL6) as an output
// and making sure the pump is turned off initially.
void waterpump_init(void) {
    DDRL |= (1 << PL6);     // Set PL6 as output (1 = output)
    PORTL &= ~(1 << PL6);   // Clear PL6 (0 = output LOW), ensures pump is OFF
}

// Simulates starting the water pump by setting PL6 high
void waterpump_start(void) {
    PORTL |= (1 << PL6);    // Set PL6 HIGH, simulates turning pump ON
}

// Simulates stopping the water pump by clearing PL6
void waterpump_stop(void) {
    PORTL &= ~(1 << PL6);   // Set PL6 LOW, simulates turning pump OFF
}
