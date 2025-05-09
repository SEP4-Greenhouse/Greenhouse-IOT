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
 
 #define LIGHT_SENSOR_ADC_CHANNEL   15
 #define LIGHT_SENSOR_ADC_MUX       ((1 << MUX2) | (1 << MUX1) | (1 << MUX0))  // ADC15 (PK7)
 #define LIGHT_SENSOR_ADC_MUX5      (1 << MUX5)
 #define ADC_TIMEOUT_CYCLES         40000UL
 
 /**
  * @brief Initialize the ADC for reading the photoresistor.
  */
 void light_init(void) {
     // Set reference voltage to AVCC (5V) and left adjust result (optional)
     ADMUX = (1 << REFS0);  // AVCC with external capacitor at AREF
 
     // Enable ADC and set prescaler to 128 (16MHz / 128 = 125kHz)
     ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
 
     // Disable digital input on ADC15 (PK7) to reduce power consumption
     DIDR2 = (1 << ADC15D);
 }
 
 /**
  * @brief Read analog value from the photoresistor.
  *
  * @return 10-bit ADC value (0-1023). Returns 0 if timeout occurs.
  */
 uint16_t light_read(void) {
     uint32_t timeout = ADC_TIMEOUT_CYCLES;
 
     // Select ADC15 (PK7)
     ADMUX = (ADMUX & 0xE0) | LIGHT_SENSOR_ADC_MUX;  // Keep REFS0, clear MUX bits
     ADCSRB = LIGHT_SENSOR_ADC_MUX5;
 
     // Start conversion
     ADCSRA |= (1 << ADSC);
 
     // Wait for conversion to complete or timeout
     while ((ADCSRA & (1 << ADSC)) && timeout--) {
         // Optional: insert _delay_us(1) if tighter control is needed
     }
 
     if (timeout == 0) {
         return 0; // Timed out
     }
 
     // Read ADC result (10-bit)
     uint16_t result = ADCL;
     result |= (ADCH << 8);
 
     return result;
 } 