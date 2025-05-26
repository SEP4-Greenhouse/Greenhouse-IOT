#include "servo.h"
#include "includes.h"

#define DDR_SERVO DDRE
#define P_SERVO   PE3

void delay_us_variable(uint16_t us) {
    while (us--) {
        _delay_us(1);  // Single-cycle fixed delay
    }
}

void servo_init(void) {
    DDR_SERVO |= (1 << P_SERVO);  // Set PE3 as output
}

void servo(uint8_t angle) {
    if (angle > 180) angle = 180;

    for (uint8_t i = 0; i < 50; i++) {
        PORTE |= (1 << P_SERVO);
        delay_us_variable(1000 + angle * 10);  // Safe workaround for variable delay
        PORTE &= ~(1 << P_SERVO);
        _delay_ms(20);
    }
}