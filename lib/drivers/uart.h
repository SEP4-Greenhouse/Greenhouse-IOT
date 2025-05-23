// /**
//  * @file uart.h
//  * @author Laurits Ivar
//  * @brief UART communication interface header.
//  * @version 0.1
//  * @date 2023-08-23
//  * 
//  * @copyright Copyright (c) 2023
//  */

// #pragma once

// #ifdef WINDOWS_TEST
//     // Skip this file completely when testing on Windows
//     #warning "Skipping drivers/uart.h because WINDOWS_TEST is defined"
// #else

// #include <stdint.h>
// #include <avr/io.h>
// #include <avr/interrupt.h>

// /**
//  * @brief Enumeration for available USART modules.
//  */
// typedef enum {
//     USART_0,    /**< USART module 0 */
//     USART_1,    /**< USART module 1 */
//     USART_2,    /**< USART module 2 */
//     USART_3     /**< USART module 3 */
// } USART_t;

// /**
//  * @brief Type definition for UART received data callback.
//  */
// typedef void (*UART_Callback_t)(uint8_t data);

// /**
//  * @brief Initialize the specified USART module.
//  * 
//  * @param usart The USART module to be initialized.
//  * @param baudrate Desired communication speed in bits per second.
//  * @param callback Callback function to handle received data, or NULL if not needed.
//  */
// void uart_init(USART_t usart, uint32_t baudrate, UART_Callback_t callback);

// /**
//  * @brief Send a single byte over UART using blocking method.
//  * 
//  * @param usart The USART module to use.
//  * @param data The byte of data to send.
//  */
// void uart_send_blocking(USART_t usart, uint8_t data);

// /**
//  * @brief Send an array of data over UART using non-blocking method.
//  * 
//  * @param usart The USART module to use.
//  * @param str Pointer to the array of data to send.
//  * @param len Number of bytes in the array to send.
//  */
// void uart_send_array_nonBlocking(USART_t usart, uint8_t *str, uint16_t len);

// /**
//  * @brief Send an array of data over UART using blocking method.
//  * 
//  * @param usart The USART module to use.
//  * @param data Pointer to the array of data to send.
//  * @param length Number of bytes in the array to send.
//  */
// void uart_send_array_blocking(USART_t usart, uint8_t *data, uint16_t length);

// /**
//  * @brief Send a null-terminated string over UART using blocking method.
//  * 
//  * @param usart The USART module to use.
//  * @param data Pointer to the null-terminated string to send.
//  */
// void uart_send_string_blocking(USART_t usart, char *data);

// /**
//  * @brief Get the currently registered receive callback for the given USART module.
//  * 
//  * @param usart The USART module.
//  * @return UART_Callback_t The function pointer to the receive callback, or NULL if none is set.
//  */
// UART_Callback_t uart_get_rx_callback(USART_t usart);
// #endif // WINDOWS_TEST

#pragma once

#include <stdint.h>  // Needed on all platforms

// Common typedefs
typedef enum {
    USART_0,
    USART_1,
    USART_2,
    USART_3
} USART_t;

typedef void (*UART_Callback_t)(uint8_t);

// --------------------------------------------------
// Function declarations (available to all platforms)
// --------------------------------------------------
void uart_init(USART_t usart, uint32_t baudrate, UART_Callback_t callback);
void uart_send_blocking(USART_t usart, uint8_t data);
void uart_send_array_nonBlocking(USART_t usart, uint8_t *str, uint16_t len);
void uart_send_array_blocking(USART_t usart, uint8_t *data, uint16_t length);
void uart_send_string_blocking(USART_t usart, const char *data);
UART_Callback_t uart_get_rx_callback(USART_t usart);

// --------------------------------------------------
// AVR-specific includes
// --------------------------------------------------
#ifndef WINDOWS_TEST
#include <avr/io.h>
#include <avr/interrupt.h>
#endif


