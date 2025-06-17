#pragma once  // Prevent multiple inclusion of this header file

#include <stdint.h>
#include "uart.h"  // For USART_t and UART_Callback_t definitions used in UART-related mock APIs

// -------------------------- Enum for Simulated Error States ----------------------------

// Represents the possible outcomes of Wi-Fi command simulations
typedef enum {
    WIFI_OK = 0,                    // Operation succeeded
    WIFI_FAIL = 1,                  // Operation failed (e.g., unable to join AP or create TCP)
    WIFI_ERROR_RECEIVED_ERROR = 2, // Received explicit "ERROR" from module
    WIFI_ERROR_NOT_RECEIVING = 3,  // No data received
    WIFI_ERROR_RECEIVING_GARBAGE = 4 // Received unexpected or invalid data
} WIFI_ERROR_MESSAGE_t;

// -------------------------- Callback Type Definition ----------------------------

// Type for callback function invoked when TCP payload is "received"
typedef void (*WIFI_TCP_Callback_t)(void);

// -------------------------- Core Mocked Wi-Fi API ----------------------------

// Initializes the mock Wi-Fi module and prepares simulated UART communication
void wifi_init(void);

// Simulates sending an "AT" command and returns the interpreted result
WIFI_ERROR_MESSAGE_t wifi_command_AT(void);

// Simulates sending credentials to join a Wi-Fi Access Point
WIFI_ERROR_MESSAGE_t wifi_command_join_AP(const char *ssid, const char *password);

// Simulates creating a TCP connection to the specified IP and port
// Registers a callback for handling incoming data and sets up the receive buffer
WIFI_ERROR_MESSAGE_t wifi_command_create_TCP_connection(const char *ip, uint16_t port, WIFI_TCP_Callback_t callback, uint8_t *recv_buffer);

// Simulates sending data over an existing TCP connection
WIFI_ERROR_MESSAGE_t wifi_command_TCP_transmit(const uint8_t *data, uint16_t length);

// Simulates disconnecting from the Access Point
WIFI_ERROR_MESSAGE_t wifi_command_quit_AP(void);

// -------------------------- Test-Only Function ----------------------------

// Injects test data into the simulated UART receive buffer
// Used for testing how the system reacts to incoming UART data (e.g., AT responses or +IPD payloads)
void simulate_uart_receive(const char *data);
