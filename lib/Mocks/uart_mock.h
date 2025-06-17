#pragma once  // Ensure the header is only included once

#include "uart.h"  // Includes definitions for USART_t (UART instance ID) and UART_Callback_t (function pointer for RX callbacks)

// Initializes the specified UART (usart) with a given baud rate.
// Also registers a callback function to be called when data is received.
void uart_init(USART_t usart, uint32_t baudrate, UART_Callback_t callback);

// Sends a null-terminated string over UART in a blocking manner.
// This means the function waits until the entire string has been transmitted before returning.
void uart_send_string_blocking(USART_t usart, const char *data);

// Sends a raw byte array over UART in a blocking manner.
// Waits for the full array to be transmitted before returning.
void uart_send_array_blocking(USART_t usart, uint8_t *data, uint16_t length);

// Sends a raw byte array over UART in a non-blocking manner.
// This returns immediately and uses interrupt-based or buffer-driven transmission (depends on mock/implementation).
void uart_send_array_nonBlocking(USART_t usart, uint8_t *data, uint16_t length);

// Retrieves the callback function that has been registered for the given UART instance.
// Useful for testing or replacing the RX handler dynamically.
UART_Callback_t uart_get_rx_callback(USART_t usart);

