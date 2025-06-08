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

static uint8_t uart_buffer[MAX_STRING_LENGTH] = {0};
static uint8_t uart_index = 0;
static volatile bool uart_done = false;

static char tcp_rx_buffer[MAX_STRING_LENGTH] = {0};
static bool tcp_string_received = false;

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

void tcp_rx(void) {
    size_t len = strlen(tcp_rx_buffer);
    tcp_rx_buffer[len] = '\r';
    tcp_rx_buffer[len + 1] = '\n';
    tcp_rx_buffer[len + 2] = '\0';
    tcp_string_received = true;
}

void send_http_post(const char* json_payload) {
    const char* host = "greenhousesep4-fsg3f5ataucugteh.swedencentral-01.azurewebsites.net";
    const char* path = "/reading";
    const char* bearer_token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."; // Token truncated for brevity

    wifi_command_create_TCP_connection(host, 80, tcp_rx, (uint8_t *)tcp_rx_buffer);

    char http_request[768];
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

    wifi_command_TCP_transmit((uint8_t*)http_request, strlen(http_request));
    uart_send_string_blocking(USART_0, "\n"); //HTTP POST sent to API
    uart_send_string_blocking(USART_0, json_payload);
    uart_send_blocking(USART_0, '\n');
}

void strip_newline(char *str) {
    for (size_t i = 0; str[i]; i++) {
        if (str[i] == '\r' || str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
}

void console_rx(uint8_t byte) {
    uart_send_blocking(USART_0, byte);
    if (byte != '\r' && byte != '\n') {
        if (uart_index < MAX_STRING_LENGTH - 1) {
            uart_buffer[uart_index++] = byte;
        }
    } else {
        uart_buffer[uart_index] = '\0';
        uart_index = 0;
        uart_done = true;
        uart_send_blocking(USART_0, '\n');
    }
}

void handle_command(const char *cmd) {
    strip_newline((char *)cmd);
    uart_send_string_blocking(USART_0, "DEBUG: Received command: ");
    uart_send_string_blocking(USART_0, cmd);
    uart_send_blocking(USART_0, '\n');

    if (strcmp(cmd, "BUTTON1") == 0) {
        control_servo_motor(0);
        control_led_on(1);
        control_display_set_number(1);
    } else if (strcmp(cmd, "BUTTON2") == 0) {
        control_servo_motor(90);
        control_led_on(2);
        control_display_set_number(2);
    } else if (strcmp(cmd, "BUTTON3") == 0) {
        control_servo_motor(180);
        control_led_on(3);
        control_display_set_number(3);
    } else if (strcmp(cmd, "LED4_ON") == 0) {
        control_led_on(4);
    } else if (strcmp(cmd, "LED4_OFF") == 0) {
        control_led_off(4);
    } else if (strcmp(cmd, "PUMP_ON") == 0) {
        control_waterpump_on();
        mqtt_publish("greenhouse/status/pump", "ON");
    } else if (strcmp(cmd, "PUMP_OFF") == 0) {
        control_waterpump_off();
        mqtt_publish("greenhouse/status/pump", "OFF");
    } else {
        int value = atoi(cmd);
        if (value || strcmp(cmd, "0") == 0) {
            control_display_set_number(value);
        } else {
            wifi_command_TCP_transmit((uint8_t *)cmd, strlen(cmd));
            uart_send_string_blocking(USART_0, "Unknown command, echoed\n");
        }
    }
}

int main(void) {
    uart_init(USART_0, 9600, console_rx);
    wifi_init();
    dht11_init();
    _delay_ms(2000);
    control_moisture_init();

#ifdef __AVR__
    sei();
#endif

    uart_send_string_blocking(USART_0, "Welcome from SEP4 IoT hardware!\n");
    wifi_command_join_AP("ONEPLUS", "00000000");
    uart_send_string_blocking(USART_0, "Wi-Fi Connected\n");

    wifi_command_create_TCP_connection("192.168.6.114", 5000, tcp_rx, (uint8_t *)tcp_rx_buffer);
    uart_send_string_blocking(USART_0, "TCP Connected to Frontend Backend\n");

    mqtt_connect("greenhouse_device_01");
    uart_send_string_blocking(USART_0, "MQTT CONNECT Sent\n");
    uart_send_string_blocking(USART_0, "Type text to send: ");

    control_leds_init();
    control_display_init();
    control_waterpump_init();
    control_display_set_number(last_valid_display);

    uint32_t loop_counter = 0;
    const uint32_t loop_threshold = READ_INTERVAL_MS / LOOP_DELAY_MS;

    while (1) {
        if (uart_done) {
            mqtt_publish("greenhouse/sensor/temp", (char *)uart_buffer);
            handle_command((char *)uart_buffer);
            uart_done = false;
            memset(uart_buffer, 0, sizeof(uart_buffer));
            uart_send_string_blocking(USART_0, "Type text to send: ");
        }

        if (tcp_string_received) {
            uart_send_string_blocking(USART_0, "TCP Received: ");
            uart_send_string_blocking(USART_0, tcp_rx_buffer);
            handle_command(tcp_rx_buffer);
            tcp_string_received = false;
        }

        if (++loop_counter >= loop_threshold) {
            loop_counter = 0;
            _delay_ms(100);

            uint8_t temp_int = 0, temp_dec = 0;
            uint8_t hum_int = 0, hum_dec = 0;
            bool success = false;

            for (int i = 0; i < 3; i++) {
                if (dht11_get(&hum_int, &hum_dec, &temp_int, &temp_dec) == DHT11_OK) {
                    success = true;
                    break;
                }
                _delay_ms(2500);
            }

            if (success) {
                last_valid_display = temp_int * 10 + (temp_dec % 10);
                control_display_set_number(last_valid_display);

                char* timestamp = get_fake_timestamp();

                char payload[256];
                snprintf(payload, sizeof(payload),
                         "{ \"timeStamp\": \"%s\", \"value\": %d, \"unit\": \"deg C\", \"sensorId\": 1 }",
                         timestamp, temp_int);
                send_http_post(payload);

                snprintf(payload, sizeof(payload),
                         "{ \"timeStamp\": \"%s\", \"value\": %d, \"unit\": \"deg C\", \"sensorId\": 2 }",
                         timestamp, hum_int);
                send_http_post(payload);

               // uint16_t moisture_raw = control_moisture_get_raw_value();
                uint8_t moisture_percent = control_moisture_get_percent();
                snprintf(payload, sizeof(payload),
                         "{ \"timeStamp\": \"%s\", \"value\": %d, \"unit\": \"deg C\", \"sensorId\": 3 }",
                         timestamp, moisture_percent);
                send_http_post(payload);
            } else {
                uart_send_string_blocking(USART_0, "DHT11 Read FAIL\n");
            }
        }

        _delay_ms(LOOP_DELAY_MS);
    }

    return 0;
}
