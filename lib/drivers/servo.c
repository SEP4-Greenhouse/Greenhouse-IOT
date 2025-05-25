/**
 * @file soil.c
 * @brief Driver for Capacitive Soil Moisture Sensor v1.2
 *
 * This file provides the implementation for initializing and reading the value
 * from a Capacitive Soil Moisture Sensor connected to pin SOIL_PIN on the ATmega2560.
 *
 * @date 09-04-2025
 * @author Erland Larsen, VIA University College
 */

#include "servo.h"
#include "includes.h"

#define DDR_SERVO   DDRE
#define P_SERVO     PE3
#define PORT_SERVO  PORTE

static uint8_t fan_state = 0;

void servo_init(void) {
    DDR_SERVO |= (1 << P_SERVO);  // Set PE3 as output
    servo_off();  // Ensure fan is off at startup
}

void servo_on(void) {
    PORT_SERVO |= (1 << P_SERVO);  // Set PE3 HIGH
    fan_state = 1;
}

void servo_off(void) {
    PORT_SERVO &= ~(1 << P_SERVO);  // Set PE3 LOW
    fan_state = 0;
}

uint8_t servo_is_on(void) {
    return fan_state;
}