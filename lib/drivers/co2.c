/**
 * @file co2.c
 * @brief CO2 sensor (e.g., MH-Z19) UART driver
 */

#include "co2.h"
#include "uart.h" // Assumes a uart_read/write API exists

void co2_init(void) {
    uart_init(9600); // Sensor default baud rate
}

uint16_t co2_read_ppm(void) {
    uint8_t cmd[9] = {0xFF, 0x01, 0x86, 0, 0, 0, 0, 0, 0x79};
    uart_write(cmd, 9);

    uint8_t response[9];
    if (uart_read(response, 9) != 9) return 0;

    if (response[0] != 0xFF || response[1] != 0x86) return 0;

    return (response[2] << 8) | response[3];
}

void co2_set_auto_calibration(bool enable) {
    uint8_t cmd[9] = {0xFF, 0x01, enable ? 0xA0 : 0xA1, 0, 0, 0, 0, 0, 0x7E};
    uart_write(cmd, 9);

    uint8_t response[9];
    if (uart_read(response, 9) != 9) return;

    if (response[0] != 0xFF || response[1] != (enable ? 0xA0 : 0xA1)) return;
}