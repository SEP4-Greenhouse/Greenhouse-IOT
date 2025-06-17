#include "uart_mock.h"  // Mock header providing function declarations and types for UART
#include <stdio.h>      // For printf and putchar (used to simulate UART output)
#include <stdint.h>     // For standard integer types

// Static variable to hold the registered RX callback function.
// This simulates interrupt-driven reception behavior in a test environment.
static UART_Callback_t rx_callback = NULL;

// Initializes the UART mock by storing the provided callback function.
// In real hardware, this would configure registers and set baud rate.
// In the mock, we only care about saving the callback for simulation.
void uart_init(USART_t usart, uint32_t baudrate, UART_Callback_t callback) {
    rx_callback = callback;
}

// Simulates sending a null-terminated string over UART (blocking).
// Instead of real transmission, prints to standard output.
void uart_send_string_blocking(USART_t usart, const char *data) {
    printf("[MOCK] uart_send_string_blocking: %s\n", data);
}

// Simulates blocking transmission of a byte array over UART.
// Prints the bytes as characters to stdout for test visibility.
void uart_send_array_blocking(USART_t usart, uint8_t *data, uint16_t length) {
    printf("[MOCK] uart_send_array_blocking: ");
    for (uint16_t i = 0; i < length; ++i) {
        putchar(data[i]);  // Output each byte as a character
    }
    putchar('\n');
}

// Simulates non-blocking transmission of a byte array.
// In real embedded code, this would likely use interrupts or DMA.
// Here, it simply prints immediately to stdout to mimic async behavior.
void uart_send_array_nonBlocking(USART_t usart, uint8_t *data, uint16_t length) {
    printf("[MOCK] uart_send_array_nonBlocking: ");
    for (uint16_t i = 0; i < length; ++i) {
        putchar(data[i]);  // Output each byte as a character
    }
    putchar('\n');
}

// Returns the previously registered RX callback for the specified UART instance.
// This allows test code to manually invoke the callback if needed.
UART_Callback_t uart_get_rx_callback(USART_t usart) {
    return rx_callback;
}
