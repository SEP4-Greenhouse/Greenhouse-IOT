#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "wifi.h"
#include "uart.h"
#include "servo_controller.h"
#include "leds_controller.h"
#include "7segment_controller.h"
#include "waterpump_controller.h"
#include "mqtt_client.h"
#include "dht11.h"
#include "moisture_controller.h"
#include "mqtt_message_factory.h"

#ifdef __AVR__
#include <util/delay.h>
#include <avr/interrupt.h>
#endif

#define MAX_STRING_LENGTH 100
#define READ_INTERVAL_MS 15000
#define LOOP_DELAY_MS 10

// Buffer to store incoming UART bytes (e.g., user commands or debug input)
static uint8_t uart_buffer[MAX_STRING_LENGTH] = {0};

// Index to track the current position in the UART input buffer
static uint8_t uart_index = 0;

// Flag indicating that a complete UART command (terminated by newline) has been received
static volatile bool uart_done = false;

// Buffer to store data received via TCP (e.g., from a remote server)
static char tcp_rx_buffer[MAX_STRING_LENGTH] = {0};

// Flag indicating that a complete TCP message has been received and is ready to process
static bool tcp_string_received = false;

// Holds the last successfully read temperature value used on the 7-segment display
// Used as a fallback in case of sensor read failure
static int last_valid_display = 234;


// Simulated timestamp tracker
char* get_fake_timestamp(void) {
    static char buffer[40];
    static unsigned long total_seconds = 0;

    total_seconds += 15;  // Add 15 seconds per loop interval

    int base_year = 2025, month = 5, day = 26;
    int hours = (total_seconds / 3600) % 24;
    int minutes = (total_seconds / 60) % 60;
    int seconds = total_seconds % 60;

    snprintf(buffer, sizeof(buffer),
             "%04d-%02d-%02dT%02d:%02d:%02d.000Z",
             base_year, month, day, hours, minutes, seconds);
    return buffer;
}

/**
 * @brief TCP receive callback function.
 * 
 * This function is called when data is received over a TCP connection.
 * It appends a CRLF ("\r\n") to the received buffer and sets a flag
 * indicating that a full TCP message has been received and is ready to process.
 */

void tcp_rx(void) {
    size_t len = strlen(tcp_rx_buffer);
    tcp_rx_buffer[len] = '\r';
    tcp_rx_buffer[len + 1] = '\n';
    tcp_rx_buffer[len + 2] = '\0';
    tcp_string_received = true;
}

/**
 * @brief Sends an HTTP POST request with a JSON payload to the cloud backend.
 *
 * This function constructs and transmits a complete HTTP/1.1 POST request
 * over a TCP connection using the ESP-based Wi-Fi module. It's used to
 * report sensor readings (e.g., temperature, humidity, moisture) to an Azure-hosted API.
 *
 * @param json_payload The JSON-formatted string to include in the request body.
 */
void send_http_post(const char* json_payload) {
    // API host and endpoint
    const char* host = "greenhousesep4-fsg3f5ataucugteh.swedencentral-01.azurewebsites.net";
    const char* path = "/reading";

    // Bearer token for authentication (truncated here for brevity)
    const char* bearer_token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...";

    // Establish a TCP connection to the host on port 80
    wifi_command_create_TCP_connection(host, 80, tcp_rx, (uint8_t *)tcp_rx_buffer);

    // Buffer to hold the complete HTTP request
    char http_request[768];

    // Format the HTTP POST request string with headers and payload
    snprintf(http_request, sizeof(http_request),
             "POST %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Authorization: Bearer %s\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             path, host, bearer_token, (int)strlen(json_payload), json_payload);

    // Transmit the HTTP request over TCP
    wifi_command_TCP_transmit((uint8_t*)http_request, strlen(http_request));

    // Optional debug output to UART
    uart_send_string_blocking(USART_0, "\n");         // Blank line
    uart_send_string_blocking(USART_0, json_payload); // Echo the sent payload
    uart_send_blocking(USART_0, '\n');                // Newline for formatting
}


/**
 * @brief Removes the first newline or carriage return character from a string.
 *
 * This is typically used to sanitize input strings received from UART or TCP,
 * where commands may end with '\r', '\n', or both.
 *
 * @param str Pointer to the null-terminated string to be sanitized.
 */
void strip_newline(char *str) {
    // Iterate through each character in the string
    for (size_t i = 0; str[i]; i++) {
        // Check for newline or carriage return
        if (str[i] == '\r' || str[i] == '\n') {
            str[i] = '\0';  // Replace it with null terminator to end the string
            break;          // Stop after removing the first line ending
        }
    }
}


/**
 * @brief UART receive callback for console input.
 *
 * This function is triggered every time a byte is received over UART.
 * It collects input into a buffer until a newline or carriage return is received,
 * then marks the input as ready for processing.
 *
 * @param byte The received byte from UART.
 */
void console_rx(uint8_t byte) {
    uart_send_blocking(USART_0, byte); // Echo the received byte back to the console

    // If the received byte is not a newline or carriage return, store it in the buffer
    if (byte != '\r' && byte != '\n') {
        if (uart_index < MAX_STRING_LENGTH - 1) {
            uart_buffer[uart_index++] = byte;  // Add byte to buffer and increment index
        }
    } else {
        // End of command detected ('\r' or '\n'), null-terminate the string
        uart_buffer[uart_index] = '\0';

        // Reset index for the next input
        uart_index = 0;

        // Set flag to indicate that a full command has been received
        uart_done = true;

        // Echo newline to indicate end of input
        uart_send_blocking(USART_0, '\n');
    }
}


/**
 * @brief Handles string-based commands received via UART or TCP.
 *
 * This function interprets user or remote commands and executes appropriate
 * control logic (e.g., moving servos, toggling LEDs, controlling the water pump,
 * updating the 7-segment display, or echoing unknown commands).
 *
 * @param cmd The null-terminated command string to be handled.
 */
void handle_command(const char *cmd) {
    // Remove trailing newline or carriage return from the command
    strip_newline((char *)cmd);

    // Debug output: show received command on the console
    uart_send_string_blocking(USART_0, "DEBUG: Received command: ");
    uart_send_string_blocking(USART_0, cmd);
    uart_send_blocking(USART_0, '\n');

    // Command: Activate servo at 0°, turn on LED 1, show "1" on display
    if (strcmp(cmd, "BUTTON1") == 0) {
        control_servo_motor(0);
        control_led_on(1);
        control_display_set_number(1);

    // Command: Activate servo at 90°, turn on LED 2, show "2" on display
    } else if (strcmp(cmd, "BUTTON2") == 0) {
        control_servo_motor(90);
        control_led_on(2);
        control_display_set_number(2);

    // Command: Activate servo at 180°, turn on LED 3, show "3" on display
    } else if (strcmp(cmd, "BUTTON3") == 0) {
        control_servo_motor(180);
        control_led_on(3);
        control_display_set_number(3);

    // Command: Turn on LED 4
    } else if (strcmp(cmd, "LED4_ON") == 0) {
        control_led_on(4);

    // Command: Turn off LED 4
    } else if (strcmp(cmd, "LED4_OFF") == 0) {
        control_led_off(4);

    // Command: Turn on water pump and publish MQTT status
    } else if (strcmp(cmd, "PUMP_ON") == 0) {
        control_waterpump_on();
        mqtt_publish("greenhouse/status/pump", "ON");

    // Command: Turn off water pump and publish MQTT status
    } else if (strcmp(cmd, "PUMP_OFF") == 0) {
        control_waterpump_off();
        mqtt_publish("greenhouse/status/pump", "OFF");

    // If the command is numeric (e.g., "42"), set the 7-segment display
    } else {
        int value = atoi(cmd);
        if (value || strcmp(cmd, "0") == 0) {
            control_display_set_number(value);
        } else {
            // Unknown command: echo it via TCP and notify via UART
            wifi_command_TCP_transmit((uint8_t *)cmd, strlen(cmd));
            uart_send_string_blocking(USART_0, "Unknown command, echoed\n");
        }
    }
}


/**
 * @brief Entry point of the SEP4 IoT firmware.
 *
 * Initializes hardware components, sets up network communication,
 * and continuously handles sensor data, command input, and cloud reporting.
 */
int main(void) {
    // Initialize UART with 9600 baud and set the receive callback to console_rx
    uart_init(USART_0, 9600, console_rx);

    // Initialize Wi-Fi module (AT command interface)
    wifi_init();

    // Initialize DHT11 temperature and humidity sensor
    dht11_init();

    // Delay to allow sensors and Wi-Fi to stabilize
    _delay_ms(2000);

    // Initialize moisture sensor subsystem
    control_moisture_init();

#ifdef __AVR__
    sei(); // Enable global interrupts (for UART or timers)
#endif

    // Greet the user over UART
    uart_send_string_blocking(USART_0, "Welcome from SEP4 IoT hardware!\n");

    // Connect to Wi-Fi Access Point
    wifi_command_join_AP("ONEPLUS", "00000000");
    uart_send_string_blocking(USART_0, "Wi-Fi Connected\n");

    // Create a TCP connection to the frontend/backend service
    wifi_command_create_TCP_connection("192.168.6.114", 5000, tcp_rx, (uint8_t *)tcp_rx_buffer);
    uart_send_string_blocking(USART_0, "TCP Connected to Frontend Backend\n");

    // Connect to MQTT broker with device ID
    mqtt_connect("greenhouse_device_01");
    uart_send_string_blocking(USART_0, "MQTT CONNECT Sent\n");

    // Prompt user for input
    uart_send_string_blocking(USART_0, "Type text to send: ");

    // Initialize hardware control modules
    control_leds_init();
    control_display_init();
    control_waterpump_init();
    control_display_set_number(last_valid_display); // Display last valid temperature

    // Variables for loop timing
    uint32_t loop_counter = 0;
    const uint32_t loop_threshold = READ_INTERVAL_MS / LOOP_DELAY_MS;

    // ------------------- Main Application Loop -------------------
    while (1) {
        // If UART command is complete, publish to MQTT and handle it
        if (uart_done) {
            mqtt_publish("greenhouse/sensor/temp", (char *)uart_buffer);
            handle_command((char *)uart_buffer); // Parse and respond to command
            uart_done = false;
            memset(uart_buffer, 0, sizeof(uart_buffer)); // Clear buffer
            uart_send_string_blocking(USART_0, "Type text to send: ");
        }

        // If a TCP message was received, process it as a command
        if (tcp_string_received) {
            uart_send_string_blocking(USART_0, "TCP Received: ");
            uart_send_string_blocking(USART_0, tcp_rx_buffer);
            handle_command(tcp_rx_buffer); // Parse and execute TCP command
            tcp_string_received = false;
        }

        // Every READ_INTERVAL_MS milliseconds, read and report sensor values
        if (++loop_counter >= loop_threshold) {
            loop_counter = 0;
            _delay_ms(100); // Slight delay before sensor reading

            // Variables for sensor readings
            uint8_t temp_int = 0, temp_dec = 0;
            uint8_t hum_int = 0, hum_dec = 0;
            bool success = false;

            // Try up to 3 times to get a valid reading from the DHT11
            for (int i = 0; i < 3; i++) {
                if (dht11_get(&hum_int, &hum_dec, &temp_int, &temp_dec) == DHT11_OK) {
                    success = true;
                    break;
                }
                _delay_ms(2500); // Wait before retrying
            }

            if (success) {
                // Update 7-segment display with temperature value (e.g., 23.4 -> 234)
                last_valid_display = temp_int * 10 + (temp_dec % 10);
                control_display_set_number(last_valid_display);

                // Create a fake timestamp for the data point
                char* timestamp = get_fake_timestamp();

                char payload[256];

                // Send temperature to the cloud
                snprintf(payload, sizeof(payload),
                         "{ \"timeStamp\": \"%s\", \"value\": %d, \"unit\": \"deg C\", \"sensorId\": 1 }",
                         timestamp, temp_int);
                send_http_post(payload);

                // Send humidity to the cloud
                snprintf(payload, sizeof(payload),
                         "{ \"timeStamp\": \"%s\", \"value\": %d, \"unit\": \"deg C\", \"sensorId\": 2 }",
                         timestamp, hum_int);
                send_http_post(payload);

                // Read and send soil moisture (percent)
                uint8_t moisture_percent = control_moisture_get_percent();
                snprintf(payload, sizeof(payload),
                         "{ \"timeStamp\": \"%s\", \"value\": %d, \"unit\": \"deg C\", \"sensorId\": 3 }",
                         timestamp, moisture_percent);
                send_http_post(payload);
            } else {
                // Sensor read failed — notify user
                uart_send_string_blocking(USART_0, "DHT11 Read FAIL\n");
            }
        }

        // Small delay for main loop timing
        _delay_ms(LOOP_DELAY_MS);
    }

    return 0; // (Never reached)
}

