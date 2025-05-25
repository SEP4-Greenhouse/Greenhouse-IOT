#include "servo_controller.h"
#include "servo.h"
#include "uart.h"

void control_servo_init(void) {
    servo_init();
    uart_send_string_blocking(USART_0, "Fan (servo) initialized\n");
}

void control_servo_on(void) {
    if (!servo_is_on()) {
        servo_on();
        uart_send_string_blocking(USART_0, "Fan turned ON\n");
    } else {
        uart_send_string_blocking(USART_0, "Fan already ON\n");
    }
}

void control_servo_off(void) {
    if (servo_is_on()) {
        servo_off();
        uart_send_string_blocking(USART_0, "Fan turned OFF\n");
    } else {
        uart_send_string_blocking(USART_0, "Fan already OFF\n");
    }
}

uint8_t control_servo_is_on(void) {
    return servo_is_on();
}