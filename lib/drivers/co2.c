/**
 * @file co2.c
 * @brief CO2 sensor driver (e.g., MH-Z19) using UART
 */

#include "co2.h"
#include "uart.h"
#include <stdbool.h>
#include <stddef.h>

void co2_init(void) {
    uart_init(USART_0, 9600, NULL);  // Set default baud rate for CO2 sensor
}

uint16_t co2_read_ppm(void) {
    uint8_t cmd[9] = {0xFF, 0x01, 0x86, 0, 0, 0, 0, 0, 0x79};
    uart_write(USART_0, cmd, 9);

    uint8_t response[9];
    if (uart_read(USART_0, response, 9) != 9) return 0;

    if (response[0] != 0xFF || response[1] != 0x86) return 0;

    return (response[2] << 8) | response[3];
}

void co2_set_auto_calibration(bool enable) {
    uint8_t cmd[9] = {0xFF, 0x01, enable ? 0xA0 : 0xA1, 0, 0, 0, 0, 0, 0x7E};
    uart_write(USART_0, cmd, 9);

    uint8_t response[9];
    if (uart_read(USART_0, response, 9) != 9) return;

    if (response[0] != 0xFF || response[1] != (enable ? 0xA0 : 0xA1)) return;
}

void co2_set_zero_point(uint16_t zero_point) {
    uint8_t cmd[9] = {0xFF, 0x01, 0xA2, (zero_point >> 8) & 0xFF, zero_point & 0xFF, 0, 0, 0, 0x7D};
    uart_write(USART_0, cmd, 9);

    uint8_t response[9];
    if (uart_read(USART_0, response, 9) != 9) return;

    if (response[0] != 0xFF || response[1] != 0xA2) return;
}