#include "servo_controller.h"
#include "servo.h"
#include "uart.h"

#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 90
#define SERVO_STEP_ANGLE 5

static uint8_t current_angle = 0;

void control_servo_init(void) {
    servo_init();
    control_servo_set_angle(0);
}

void control_servo_set_angle(uint8_t angle) {
    if (angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;
    current_angle = angle;
    servo(current_angle);
}

void control_servo_step_clockwise(void) {
    if (current_angle >= SERVO_MAX_ANGLE) {
        uart_send_string_blocking(USART_0, "[SERVO] Already fully open (90°)");
        return;
    }
    control_servo_set_angle(current_angle + SERVO_STEP_ANGLE);
}

void control_servo_step_counterclockwise(void) {
    if (current_angle <= SERVO_MIN_ANGLE) {
        uart_send_string_blocking(USART_0, "[SERVO] Already fully closed (0°)");
        return;
    }
    control_servo_set_angle(current_angle - SERVO_STEP_ANGLE);
}

uint8_t control_servo_get_angle(void) {
    return current_angle;
}

const char* control_servo_get_position_status(void) {
    if (current_angle == 0) return "Completely Closed";
    if (current_angle == 90) return "Completely Open";
    if (current_angle < 30) return "Slightly Open";
    if (current_angle < 60) return "Open";
    return "Wide Open";
}