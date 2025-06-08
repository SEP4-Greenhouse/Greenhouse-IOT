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
    // Read the raw analog-to-digital converter (ADC) value from the moisture sensor
    uint16_t adc = control_moisture_get_raw_value();

    // Ensure the ADC value doesn't exceed the maximum 10-bit value (1023)
    if (adc > 1023) adc = 1023;

    // Loop through predefined ADC ranges in moisture_map to find which range the current adc fits
    for (uint8_t i = 0; i < 3; ++i) {
        // If adc is greater than or equal to the next value in the map, we found the segment to interpolate within
        if (adc >= moisture_map[i + 1]) {
            // Define the lower and upper bounds of the current segment
            uint16_t x0 = moisture_map[i];
            uint16_t x1 = moisture_map[i + 1];

            // Corresponding percentage moisture values for x0 and x1
            uint8_t y0 = moisture_percent_map[i];
            uint8_t y1 = moisture_percent_map[i + 1];

            // Perform linear interpolation to estimate moisture percentage from ADC value
            uint8_t percent = y0 + ((int32_t)(adc - x0) * (y1 - y0)) / (x1 - x0);

#ifdef __AVR__
            // If on AVR hardware, get moisture level label and print debug message
            const char* label = control_moisture_get_level(adc);
            char debug_msg[96];
            sprintf(debug_msg, "[DEBUG] Moisture: %u ADC, ~%u%% (%s)\n", adc, percent, label);
            uart_send_string_blocking(USART_0, debug_msg);
#endif
            // Return the interpolated moisture percentage
            return percent;
        }
    }

#ifdef __AVR__
    // If no match found in any range
    uart_send_string_blocking(USART_0, "[WARN] ADC value did not match interpolation range\n");
#endif

    // Fallback: return last element in moisture_percent_map (likely 100% if soil is soaked)
    return moisture_percent_map[3];
}


const char* control_moisture_get_level(uint16_t adc_value) {
    for (uint8_t i = 0; i < 4; ++i) {
        if (adc_value >= moisture_map[i]) {
            return moisture_label_map[i];
        }
    }
    return "Unknown";
}
