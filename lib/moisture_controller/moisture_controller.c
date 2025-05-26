#include "moisture_controller.h"
#include <avr/io.h>

#ifdef __AVR__
#include <util/delay.h>
#include "uart.h"
#endif

#define MOISTURE_ADC_CHANNEL 0  // Adjust as needed

// Moisture level thresholds
const uint16_t moisture_map[] = {900, 700, 500, 0};
const uint8_t moisture_percent_map[] = {2, 30, 60, 100};
const char* moisture_label_map[] = {"Dry", "Slightly Wet", "Very Wet", "Soaked"};

void control_moisture_init(void) {
    ADMUX = (1 << REFS0); // Use AVcc as reference
    ADCSRA = (1 << ADEN)  // Enable ADC
           | (1 << ADPS2) | (1 << ADPS1); // Prescaler = 64

#ifdef __AVR__
    uart_send_string_blocking(USART_0, "[INIT] Moisture sensor ADC initialized\n");
#endif
}

uint16_t control_moisture_get_raw_value(void) {
    ADMUX = (ADMUX & 0xF0) | (MOISTURE_ADC_CHANNEL & 0x0F);  // Select channel
#ifdef __AVR__
    _delay_us(10); // Settling time
#endif

    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC)); // Wait for result

    uint16_t adc_value = ADC;

#ifdef __AVR__
    char debug_msg[64];
    sprintf(debug_msg, "[DEBUG] ADC raw read: %u (channel: %u)\n", adc_value, MOISTURE_ADC_CHANNEL);
    uart_send_string_blocking(USART_0, debug_msg);
#endif

    return adc_value;
}

uint8_t control_moisture_get_percent(void) {
    uint16_t adc = control_moisture_get_raw_value();
    if (adc > 1023) adc = 1023;

    for (uint8_t i = 0; i < 4; ++i) {
        if (adc >= moisture_map[i]) {
#ifdef __AVR__
            const char* label = moisture_label_map[i];
            char debug_msg[96];
            sprintf(debug_msg, "[DEBUG] Moisture: %u ADC, %u%% (%s)\n", adc, moisture_percent_map[i], label);
            uart_send_string_blocking(USART_0, debug_msg);
#endif
            return moisture_percent_map[i];
        }
    }

#ifdef __AVR__
    uart_send_string_blocking(USART_0, "[WARN] ADC value did not match any moisture range\n");
#endif
    return 0;
}

const char* control_moisture_get_level(uint16_t adc_value) {
    for (uint8_t i = 0; i < 4; ++i) {
        if (adc_value >= moisture_map[i]) {
            return moisture_label_map[i];
        }
    }
    return "Unknown";
}
