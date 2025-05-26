#include "button_controller.h"
#include <avr/io.h>

#define BTN_S1_PIN PA1
#define BTN_S2_PIN PA2
#define BTN_S3_PIN PA3
#define BTN_S4_PIN PA4

void control_buttons_init(void) {
    DDRA &= ~((1 << BTN_S1_PIN) | (1 << BTN_S2_PIN) | (1 << BTN_S3_PIN) | (1 << BTN_S4_PIN));
    PORTA |= (1 << BTN_S1_PIN) | (1 << BTN_S2_PIN) | (1 << BTN_S3_PIN) | (1 << BTN_S4_PIN);  // Enable pull-ups
}

uint8_t control_button_s1_pressed(void) {
    return !(PINA & (1 << BTN_S1_PIN));
}
uint8_t control_button_s2_pressed(void) {
    return !(PINA & (1 << BTN_S2_PIN));
}
uint8_t control_button_s3_pressed(void) {
    return !(PINA & (1 << BTN_S3_PIN));
}
uint8_t control_button_s4_pressed(void) {
    return !(PINA & (1 << BTN_S4_PIN));
}