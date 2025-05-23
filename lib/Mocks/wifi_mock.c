#include "wifi_mock.h"

#ifdef WINDOWS_TEST
#include "uart_mock.h"
#else
#include "uart.h"
#endif

#include <string.h>
#include <stdio.h>

static char rx_buffer[128];
static int rx_index = 0;
static WIFI_TCP_Callback_t tcp_callback = NULL;
static uint8_t *tcp_buffer = NULL;

void wifi_init(void) {
    uart_init(USART_2, 115200, NULL);
    rx_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));
}

WIFI_ERROR_MESSAGE_t wifi_command_AT(void) {
    uart_send_string_blocking(USART_2, "AT\r\n");
    if (strstr(rx_buffer, "OK")) return WIFI_OK;
    if (strstr(rx_buffer, "ERROR")) return WIFI_ERROR_RECEIVED_ERROR;
    if (rx_index == 0) return WIFI_ERROR_NOT_RECEIVING;
    return WIFI_ERROR_RECEIVING_GARBAGE;
}

WIFI_ERROR_MESSAGE_t wifi_command_join_AP(const char *ssid, const char *password) {
    uart_send_string_blocking(USART_2, "AT+JOIN_AP\r\n");
    if (strstr(rx_buffer, "OK")) return WIFI_OK;
    if (strstr(rx_buffer, "FAIL")) return WIFI_FAIL;
    return WIFI_ERROR_RECEIVING_GARBAGE;
}

WIFI_ERROR_MESSAGE_t wifi_command_create_TCP_connection(const char *ip, uint16_t port, WIFI_TCP_Callback_t cb, uint8_t *recv_buf) {
    uart_send_string_blocking(USART_2, "AT+TCP_CONN\r\n");
    tcp_callback = cb;
    tcp_buffer = recv_buf;
    if (strstr(rx_buffer, "OK")) return WIFI_OK;
    if (strstr(rx_buffer, "FAIL")) return WIFI_FAIL;
    return WIFI_ERROR_RECEIVING_GARBAGE;
}

WIFI_ERROR_MESSAGE_t wifi_command_TCP_transmit(const uint8_t *data, uint16_t length) {
    uart_send_array_blocking(USART_2, (uint8_t *)data, length);
    return WIFI_OK;
}

WIFI_ERROR_MESSAGE_t wifi_command_quit_AP(void) {
    uart_send_string_blocking(USART_2, "AT+QUIT_AP\r\n");
    return WIFI_OK;
}

void simulate_uart_receive(const char *data) {
    for (int i = 0; data[i] != '\0' && rx_index < (int)(sizeof(rx_buffer) - 1); ++i) {
        rx_buffer[rx_index++] = data[i];
    }
    rx_buffer[rx_index] = '\0';

    // Extract TCP payload if present
    const char *ipd = strstr(rx_buffer, "+IPD,4:");
    if (ipd && tcp_callback && tcp_buffer) {
        const char *payload = ipd + strlen("+IPD,4:");
        memcpy(tcp_buffer, payload, 4);
        tcp_buffer[4] = '\0';
        tcp_callback();
    }
}

