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

void tcp_rx(void) {
    size_t len = strlen(tcp_rx_buffer);
    tcp_rx_buffer[len] = '\r';
    tcp_rx_buffer[len + 1] = '\n';
    tcp_rx_buffer[len + 2] = '\0';
    tcp_string_received = true;
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
    control_moisture_init();

#ifdef __AVR__
    sei();
#endif

    uart_send_string_blocking(USART_0, "Welcome from SEP4 IoT hardware!\n");
    wifi_command_join_AP("ONEPLUS", "00000000");
    uart_send_string_blocking(USART_0, "Wi-Fi Connected\n");

    wifi_command_create_TCP_connection("192.168.219.114", 5000, tcp_rx, (uint8_t *)tcp_rx_buffer);
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

            // Temperature sensor read
            uint8_t temp_int = 0, temp_dec = 0;
            bool success = false;
            for (int i = 0; i < 3; i++) {
                if (dht11_get(NULL, NULL, &temp_int, &temp_dec) == DHT11_OK) {
                    success = true;
                    break;
                }
                _delay_ms(2500);
            }

            if (success) {
                last_valid_display = temp_int * 10 + (temp_dec % 10);
                char msg[64];
                sprintf(msg, "TEMP: %d.%dC\n", temp_int, temp_dec);
                uart_send_string_blocking(USART_0, msg);
                wifi_command_TCP_transmit((uint8_t *)msg, strlen(msg));
                control_display_set_number(last_valid_display);
            } else {
                uart_send_string_blocking(USART_0, "DHT11 Read FAIL\n");
            }

            // Moisture read and debug
            uint16_t moisture_raw = control_moisture_get_raw_value();
            uint8_t moisture_percent = control_moisture_get_percent();

            const char* level = control_moisture_get_level(moisture_raw);
            char moist_msg[96];
            sprintf(moist_msg, "[MOISTURE] ADC: %u, %u%% (%s)\n", moisture_raw, moisture_percent, level);
            uart_send_string_blocking(USART_0, moist_msg);

            char moist_mqtt[32];
            sprintf(moist_mqtt, "%u", moisture_percent);
            mqtt_publish("greenhouse/sensor/moisture", moist_mqtt);
        }

        _delay_ms(LOOP_DELAY_MS);
    }

    return 0;
}
