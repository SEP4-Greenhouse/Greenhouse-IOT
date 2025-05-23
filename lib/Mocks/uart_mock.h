#pragma once
#include "uart.h"  // Includes the official USART_t and UART_Callback_t definitions

void uart_init(USART_t usart, uint32_t baudrate, UART_Callback_t callback);
void uart_send_string_blocking(USART_t usart, const char *data);
void uart_send_array_blocking(USART_t usart, uint8_t *data, uint16_t length);
void uart_send_array_nonBlocking(USART_t usart, uint8_t *data, uint16_t length);
UART_Callback_t uart_get_rx_callback(USART_t usart);
