#include "servo_controller.h"
#include "servo.h"

#ifdef __AVR__
#include <util/delay.h>
#endif

void control_servo_init(void) {
    // Optional placeholder for initialization
}

void control_servo_set_angle(uint8_t angle) {
    servo(angle);
}

void control_servo_sweep(void) {
    for (uint8_t angle = 0; angle <= 180; angle += 5) {
        servo(angle);
#ifdef __AVR__
        _delay_ms(50);
#endif
    }
    for (uint8_t angle = 180; angle > 0; angle -= 5) {
        servo(angle);
#ifdef __AVR__
        _delay_ms(50);
#endif
    }
}