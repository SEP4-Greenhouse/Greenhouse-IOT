#pragma once

#include <stdint.h>
#include "uart.h"  // For USART_t, UART_Callback_t

typedef enum {
    WIFI_OK = 0,
    WIFI_FAIL = 1,
    WIFI_ERROR_RECEIVED_ERROR = 2,
    WIFI_ERROR_NOT_RECEIVING = 3,
    WIFI_ERROR_RECEIVING_GARBAGE = 4
} WIFI_ERROR_MESSAGE_t;

typedef void (*WIFI_TCP_Callback_t)(void);

// Core mocked functions
void wifi_init(void);
WIFI_ERROR_MESSAGE_t wifi_command_AT(void);
WIFI_ERROR_MESSAGE_t wifi_command_join_AP(const char *ssid, const char *password);
WIFI_ERROR_MESSAGE_t wifi_command_create_TCP_connection(const char *ip, uint16_t port, WIFI_TCP_Callback_t callback, uint8_t *recv_buffer);
WIFI_ERROR_MESSAGE_t wifi_command_TCP_transmit(const uint8_t *data, uint16_t length);
WIFI_ERROR_MESSAGE_t wifi_command_quit_AP(void);

// For test simulation
void simulate_uart_receive(const char *data);
