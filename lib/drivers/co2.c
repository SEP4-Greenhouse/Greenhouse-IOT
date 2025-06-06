/**
 * @file co2.c
 * @brief CO2 sensor driver (e.g., MH-Z19) using UART
 */

#include "co2.h"
#include "uart.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Initialize UART communication for CO2 sensor
 *
 * Typically, MH-Z19 runs at 9600 baud
 */
void co2_init(void) {
    uart_init(USART_0, 9600, NULL);  // Set default baud rate for CO2 sensor
}

/**
 * @brief Read CO2 concentration in ppm from sensor
 *
 * @return CO2 concentration in ppm (0 if failed)
 */
uint16_t co2_read_ppm(void) {
    // Command to request CO2 reading
    uint8_t cmd[9] = {0xFF, 0x01, 0x86, 0, 0, 0, 0, 0, 0x79};
    uart_write(USART_0, cmd, 9);

    // Response should be 9 bytes
    uint8_t response[9];
    if (uart_read(USART_0, response, 9) != 9) return 0;

    // Validate response format
    if (response[0] != 0xFF || response[1] != 0x86) return 0;

    // Extract CO2 ppm from response (high byte << 8 + low byte)
    return (response[2] << 8) | response[3];
}

// Enables or disables auto-calibration feature of the CO2 sensor
void co2_set_auto_calibration(bool enable) {
    // Send calibration command with A0 (disable) or A1 (enable)
    uint8_t cmd[9] = {0xFF, 0x01, enable ? 0xA0 : 0xA1, 0, 0, 0, 0, 0, 0x7E};
    uart_write(USART_0, cmd, 9);

    // Buffer for response
    uint8_t response[9];
    if (uart_read(USART_0, response, 9) != 9) return;

    // Confirm the response type matches the intended command
    if (response[0] != 0xFF || response[1] != (enable ? 0xA0 : 0xA1)) return;
}

// Sets the sensor's zero reference point (baseline CO2 level)
void co2_set_zero_point(uint16_t zero_point) {
    // Build command with zero_point split into high and low bytes
    uint8_t cmd[9] = {0xFF, 0x01, 0xA2, (zero_point >> 8) & 0xFF, zero_point & 0xFF, 0, 0, 0, 0x7D};
    uart_write(USART_0, cmd, 9); // Send to sensor

    // Wait for acknowledgment
    uint8_t response[9];
    if (uart_read(USART_0, response, 9) != 9) return;

    // Validate expected response
    if (response[0] != 0xFF || response[1] != 0xA2) return;
}