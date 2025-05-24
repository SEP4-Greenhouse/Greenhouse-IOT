#include "servo_controller.h"
#include "servo.h"
#include "uart.h"

#ifdef __AVR__
#include <util/delay.h>
#endif

void control_servo_motor(uint8_t angle) {
    control_servo_set_angle(angle);
}

void control_servo_init(void) {
    servo_init();  // Sets pin direction
}

void control_servo_set_angle(uint8_t angle) {
    if (angle > 180) angle = 180;
    char msg[40];
    sprintf(msg, "Servo: moving to %u degrees\n", angle);
    uart_send_string_blocking(USART_0, msg);
    servo(angle);
}

void control_servo_sweep(void) {
    for (uint8_t angle = 0; angle <= 180; angle += 5) {
        control_servo_set_angle(angle);
#ifdef __AVR__
        _delay_ms(50);
#endif
    }
    for (uint8_t angle = 180; angle > 0; angle -= 5) {
        control_servo_set_angle(angle);
#ifdef __AVR__
        _delay_ms(50);
#endif
    }
}