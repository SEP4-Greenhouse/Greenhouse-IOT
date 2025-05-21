/**
 * @file pir.c
 * @brief PIR motion sensor driver using external interrupt INT2
 */

#include "pir.h"
#include "includes.h"

#define PIR_DDR  DDRD
#define PIR_PORT PORTD
#define PIR_PIN  PIND
#define PIR_BIT  PD2

static pir_callback_t pir_callback = NULL;

#ifndef WINDOWS_TEST
ISR(INT2_vect) {
    if (pir_callback) {
        pir_callback();
    }
}
#endif

void pir_init(pir_callback_t callback) {
    PIR_DDR &= ~(1 << PIR_BIT);
    PIR_PORT |= (1 << PIR_BIT);

    EICRA = (EICRA & ~((1 << ISC21) | (1 << ISC20))) | ((1 << ISC21) | (1 << ISC20));
    EIMSK |= (1 << INT2);

    pir_callback = callback;

    sei();
}

void pir_deinit(void) {
    EIMSK &= ~(1 << INT2);
    pir_callback = NULL;
}

uint8_t pir_is_motion_detected(void) {
    return (PIR_PIN & (1 << PIR_BIT)) ? 1 : 0;
}