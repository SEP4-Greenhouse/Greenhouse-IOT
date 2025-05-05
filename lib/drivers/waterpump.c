#include "waterpump.h"
#include "includes.h"  // Assuming this includes F_CPU and IO definitions

// Define the control pin
#define PUMP_PORT PORTL
#define PUMP_DDR  DDRL
#define PUMP_PIN  PL6

void waterpump_init(void)
{
    PUMP_DDR |= (1 << PUMP_PIN); // Set pump pin as output
    PUMP_PORT &= ~(1 << PUMP_PIN); // Make sure pump is off initially
}

void waterpump_start(void)
{
    PUMP_PORT |= (1 << PUMP_PIN); // Turn pump ON
}

void waterpump_stop(void)
{
    PUMP_PORT &= ~(1 << PUMP_PIN); // Turn pump OFF
}