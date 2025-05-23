/**
 * @file co2.c
 * @brief CO2 sensor driver (e.g., MH-Z19) using UART
 */

#include "co2.h"
#include "uart.h"         // Assumes uart_read/write API is available
#include <stdbool.h>      // For using bool, true, false
#include <stddef.h>       // This defines NULL
#define USART0 0

void co2_init(void) {
    uart_init(USART0, 9600, NULL);  // Set default baud rate for CO2 sensor
}

/**
 * @brief Reads CO2 concentration (ppm) from the sensor.
 *
 * @return CO2 concentration in ppm. Returns 0 on error.
 */
uint16_t co2_read_ppm(void) {
    uint8_t cmd[9] = {0xFF, 0x01, 0x86, 0, 0, 0, 0, 0, 0x79};
    uart_write(USART0, cmd, 9);

    uint8_t response[9];
    if (uart_read(USART0, response, 9) != 9) return 0;

    if (response[0] != 0xFF || response[1] != 0x86) return 0;

    return (response[2] << 8) | response[3];
}

/**
 * @brief Enables or disables automatic baseline calibration.
 *
 * @param enable Pass `true` to enable, `false` to disable.
 */
void co2_set_auto_calibration(bool enable) {
    uint8_t cmd[9] = {0xFF, 0x01, enable ? 0xA0 : 0xA1, 0, 0, 0, 0, 0, 0x7E};
    uart_write(USART0, cmd, 9);

    uint8_t response[9];
    if (uart_read(USART0, response, 9) != 9) return;

    if (response[0] != 0xFF || response[1] != (enable ? 0xA0 : 0xA1)) return;
}