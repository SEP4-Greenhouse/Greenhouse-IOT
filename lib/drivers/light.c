/**
 * @file light.c
 * @brief Photoresistor (LDR) driver implementation for ATmega2560
 *
 * Provides initialization and analog value reading from a photoresistor
 * connected to analog pin PK7 (ADC15).
 *
 * @author
 * @date April 2025
 */

#include "light.h"
#include "includes.h"
//A15 PK7!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/**
 * @brief Initialize ADC for photoresistor
 *
 * This function initializes the ADC to read values from the photoresistor connected to pin PK1 (ADC9).
 */
void light_init(void) {

    //Vcc
    //DDRK|=(1 << PK2);
    //PORTK|=(1 << PK2);

    //GND
    //DDRK|=(1 << PK1);

    // Set reference voltage to AVCC and left adjust ADC result
    ADMUX = (1 << REFS0);
    // Enable ADC and set prescaler to 64 (16MHz/128 = 125kHz)
    // ADC must operate between 50kHz and 200kHz for its full 10-bit resolution
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1)| (1 << ADPS0);
    

    // Disable digital input on PK2 (ADC10) (page 287)
    // This will reduce power consumption on the pin
    DIDR2 = (1 << ADC15D);

}

/**
 * @brief Read value from photoresistor
 *
 * This function reads the ADC value from the photoresistor connected to pin PK1 (ADC9).
 *
 * @return 10-bit ADC value read from the photoresistor
 */
uint16_t light_read(void) {
    uint32_t timeout = 40000;  // ~5ms timeout

    // Clear previous channel selection (keep only REFS bits)
    ADMUX &= 0xE0;  // Clear MUX0–MUX4 (bits 0–4)
    ADMUX |= (1 << MUX2) | (1 << MUX1) | (1 << MUX0);  // Set MUX[2:0] = 111 (bits 2:0)
    ADCSRB |= (1 << MUX5);  // Set MUX5 = 1 (for channels 8–15)

    // Start conversion
    ADCSRA |= (1 << ADSC);

    // Wait for conversion to complete or timeout
    while ((ADCSRA & (1 << ADSC)) && timeout > 0) {
        timeout--;
    }

    // Read result
    uint16_t adc_value = ADCL;
    adc_value |= (ADCH << 8);

    return adc_value;
}
