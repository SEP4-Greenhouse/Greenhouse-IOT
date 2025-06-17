#include "wifi_mock.h"  // Mocked Wi-Fi interface header

// Conditional inclusion depending on whether we're testing in a Windows environment
#ifdef WINDOWS_TEST
#include "uart_mock.h"  // Use mocked UART implementation for Windows tests
#else
#include "uart.h"       // Use actual UART interface for embedded target
#endif

#include <string.h>     // For string operations (strstr, memset, etc.)
#include <stdio.h>      // For I/O (not strictly needed here, but useful in mocks)

// -------------------- Static Variables for Internal State ---------------------

static char rx_buffer[128];           // Simulated RX buffer to hold incoming UART data
static int rx_index = 0;              // Index into rx_buffer
static WIFI_TCP_Callback_t tcp_callback = NULL;  // Callback for TCP payload reception
static uint8_t *tcp_buffer = NULL;    // Pointer to mock TCP receive buffer

// -------------------- Mock Wi-Fi Command Implementations ----------------------

// Initialize Wi-Fi module mock (sets up UART and clears buffer)
void wifi_init(void) {
    uart_init(USART_2, 115200, NULL);     // Initialize mock UART with dummy baudrate
    rx_index = 0;                         // Reset buffer index
    memset(rx_buffer, 0, sizeof(rx_buffer));  // Clear simulated RX buffer
}

// Simulate sending "AT" command and interpreting the response
WIFI_ERROR_MESSAGE_t wifi_command_AT(void) {
    uart_send_string_blocking(USART_2, "AT\r\n");  // Simulate command transmission
    if (strstr(rx_buffer, "OK")) return WIFI_OK;                       // Success response
    if (strstr(rx_buffer, "ERROR")) return WIFI_ERROR_RECEIVED_ERROR; // Explicit error
    if (rx_index == 0) return WIFI_ERROR_NOT_RECEIVING;               // No data at all
    return WIFI_ERROR_RECEIVING_GARBAGE;                              // Unexpected data
}

// Simulate joining a Wi-Fi network (with fake SSID and password)
WIFI_ERROR_MESSAGE_t wifi_command_join_AP(const char *ssid, const char *password) {
    uart_send_string_blocking(USART_2, "AT+JOIN_AP\r\n");  // Simulate join command
    if (strstr(rx_buffer, "OK")) return WIFI_OK;
    if (strstr(rx_buffer, "FAIL")) return WIFI_FAIL;
    return WIFI_ERROR_RECEIVING_GARBAGE;
}

// Simulate creating a TCP connection, with a callback to handle incoming payloads
WIFI_ERROR_MESSAGE_t wifi_command_create_TCP_connection(const char *ip, uint16_t port, WIFI_TCP_Callback_t cb, uint8_t *recv_buf) {
    uart_send_string_blocking(USART_2, "AT+TCP_CONN\r\n");  // Simulate TCP connection command
    tcp_callback = cb;      // Store callback for use in simulated receive
    tcp_buffer = recv_buf;  // Store buffer where received payload will be written
    if (strstr(rx_buffer, "OK")) return WIFI_OK;
    if (strstr(rx_buffer, "FAIL")) return WIFI_FAIL;
    return WIFI_ERROR_RECEIVING_GARBAGE;
}

// Simulate sending TCP data over the established connection
WIFI_ERROR_MESSAGE_t wifi_command_TCP_transmit(const uint8_t *data, uint16_t length) {
    uart_send_array_blocking(USART_2, (uint8_t *)data, length);  // Simulate TX
    return WIFI_OK;
}

// Simulate disconnecting from the Access Point
WIFI_ERROR_MESSAGE_t wifi_command_quit_AP(void) {
    uart_send_string_blocking(USART_2, "AT+QUIT_AP\r\n");  // Simulate quit command
    return WIFI_OK;
}

// ------------------- Test-Only Helper: Simulate Incoming Data ------------------

// Simulates reception of data over UART (e.g., test-controlled input)
void simulate_uart_receive(const char *data) {
    // Load the incoming characters into the simulated RX buffer
    for (int i = 0; data[i] != '\0' && rx_index < (int)(sizeof(rx_buffer) - 1); ++i) {
        rx_buffer[rx_index++] = data[i];
    }
    rx_buffer[rx_index] = '\0';  // Null-terminate the buffer

    // Check if the input contains a mock "+IPD" TCP payload
    const char *ipd = strstr(rx_buffer, "+IPD,4:");
    if (ipd && tcp_callback && tcp_buffer) {
        const char *payload = ipd + strlen("+IPD,4:");  // Get pointer to payload data
        memcpy(tcp_buffer, payload, 4);                 // Copy payload to buffer
        tcp_buffer[4] = '\0';                           // Null-terminate the buffer
        tcp_callback();                                 // Invoke the registered callback
    }
}
