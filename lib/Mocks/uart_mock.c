#include "uart_mock.h"
#include <stdio.h>
#include <stdint.h>

static UART_Callback_t rx_callback = NULL;

void uart_init(USART_t usart, uint32_t baudrate, UART_Callback_t callback) {
    rx_callback = callback;
}

void uart_send_string_blocking(USART_t usart, const char *data) {
    printf("[MOCK] uart_send_string_blocking: %s\n", data);
}

void uart_send_array_blocking(USART_t usart, uint8_t *data, uint16_t length) {
    printf("[MOCK] uart_send_array_blocking: ");
    for (uint16_t i = 0; i < length; ++i) {
        putchar(data[i]);
    }
    putchar('\n');
}

void uart_send_array_nonBlocking(USART_t usart, uint8_t *data, uint16_t length) {
    printf("[MOCK] uart_send_array_nonBlocking: ");
    for (uint16_t i = 0; i < length; ++i) {
        putchar(data[i]);
    }
    putchar('\n');
}

UART_Callback_t uart_get_rx_callback(USART_t usart) {
    return rx_callback;
}
