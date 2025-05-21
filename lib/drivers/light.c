/**
 * @file light.c
 * @brief Photoresistor (LDR) driver implementation for ATmega2560
 *
 * Provides initialization and analog value reading from a photoresistor
 * connected to analog pin PK7 (ADC15).
 */

#include "light.h"
#include "includes.h"

/**
 * @brief Select an ADC channel (0–31) for input.
 *
 * @param channel ADC channel to select (0–31)
 */
static void select_adc_channel(uint8_t channel) {
    ADMUX = (ADMUX & 0xE0) | (channel & 0x1F);        // Set MUX[4:0]
    ADCSRB = (ADCSRB & ~(1 << MUX5)) |                // Set MUX5 if needed
             ((channel & 0x20) >> 2);
}

/**
 * @brief Initialize ADC for photoresistor on ADC15 (PK7).
 */
void light_init(void) {
    // Set reference voltage to AVCC and left adjust result
    ADMUX = (1 << REFS0); // AVCC reference

    // Enable ADC, set prescaler to 128 -> ADC frequency = 16MHz / 128 = 125kHz
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // Disable digital input on ADC15 to save power (PK7)
    DIDR2 |= (1 << ADC15D);
}

/**
 * @brief Read analog value from the photoresistor.
 *
 * @return 10-bit ADC value (0–1023)
 */
uint16_t light_read(void) {
    uint32_t timeout = 40000; // Timeout loop count (~5ms)

    select_adc_channel(15); // ADC15 = PK7

    // Start ADC conversion
    ADCSRA |= (1 << ADSC);

    // Wait for conversion to complete or timeout
    while ((ADCSRA & (1 << ADSC)) && --timeout);

    // If timeout occurred, return 0 as error value
    if (timeout == 0) {
        return 0;
    }

    // Read result (ADCL must be read before ADCH)
    uint16_t adc_value = ADCL;
    adc_value |= (ADCH << 8);

    return adc_value;
}

/**
 * @brief Deinitialize ADC to save power if no longer needed.
 */
void light_deinit(void) {
    ADCSRA &= ~(1 << ADEN); // Disable ADC
}