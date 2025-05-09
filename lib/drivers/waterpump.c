#include <stdint.h>
#include "waterpump.h"

uint8_t PORTL = 0;
uint8_t DDRL = 0;
#define PL6 6

void waterpump_init(void)
{
    DDRL |= (1 << PL6);
    PORTL &= ~(1 << PL6);
}

void waterpump_start(void)
{
    PORTL |= (1 << PL6);
}

void waterpump_stop(void)
{
    PORTL &= ~(1 << PL6);
}
