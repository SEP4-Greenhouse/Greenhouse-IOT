#include <stdint.h>
#include "waterpump.h"

// Simulated AVR IO registers for unit testing only
uint8_t PORTL = 0;
uint8_t DDRL = 0;
#define PL6 6

void waterpump_init(void)
{
    DDRL |= (1 << PL6);       // Set PL6 as output
    PORTL &= ~(1 << PL6);     // Ensure pump is off
}

void waterpump_start(void)
{
    PORTL |= (1 << PL6);      // Turn on pump (PL6 high)
}

void waterpump_stop(void)
{
    PORTL &= ~(1 << PL6);     // Turn off pump (PL6 low)
}
