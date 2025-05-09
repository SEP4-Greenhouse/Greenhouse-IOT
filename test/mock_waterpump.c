#include <stdint.h>
#include "waterpump.h"

// Simulated AVR IO registers
uint8_t DDRC = 0;
uint8_t PORTC = 0;

// Simulated pin for water pump relay control
#define PC7 7

void waterpump_init(void)
{
    DDRC |= (1 << PC7);       // Set PC7 as output
    PORTC &= ~(1 << PC7);     // Ensure pump is off
}

void waterpump_start(void)
{
    PORTC |= (1 << PC7);      // Turn on relay (pump on)
}

void waterpump_stop(void)
{
    PORTC &= ~(1 << PC7);     // Turn off relay (pump off)
}
