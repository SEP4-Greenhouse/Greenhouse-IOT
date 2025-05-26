#include "moisture_controller.h"
#include <avr/io.h>
#include "soil.h"

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
    soil_init();

#ifdef __AVR__
    uart_send_string_blocking(USART_0, "[INIT] Moisture sensor ADC initialized\n");
#endif
}

uint16_t control_moisture_get_raw_value(void) {
    uint16_t adc_value;
    adc_value = soil_read();

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

    // Iterate through moisture_map to find the appropriate segment
    for (uint8_t i = 0; i < 3; ++i) {
        if (adc >= moisture_map[i + 1]) {
            uint16_t x0 = moisture_map[i];
            uint16_t x1 = moisture_map[i + 1];
            uint8_t y0 = moisture_percent_map[i];
            uint8_t y1 = moisture_percent_map[i + 1];

            // Linear interpolation
            uint8_t percent = y0 + ((int32_t)(adc - x0) * (y1 - y0)) / (x1 - x0);

#ifdef __AVR__
            const char* label = control_moisture_get_level(adc);
            char debug_msg[96];
            sprintf(debug_msg, "[DEBUG] Moisture: %u ADC, ~%u%% (%s)\n", adc, percent, label);
            uart_send_string_blocking(USART_0, debug_msg);
#endif
            return percent;
        }
    }

#ifdef __AVR__
    uart_send_string_blocking(USART_0, "[WARN] ADC value did not match interpolation range\n");
#endif
    return moisture_percent_map[3]; // Assume highest percentage for lowest range (e.g. "Soaked")
}


const char* control_moisture_get_level(uint16_t adc_value) {
    for (uint8_t i = 0; i < 4; ++i) {
        if (adc_value >= moisture_map[i]) {
            return moisture_label_map[i];
        }
    }
    return "Unknown";
}
