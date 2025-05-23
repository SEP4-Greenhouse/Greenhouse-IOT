// #include "wifi.h"
// #include "uart.h"
// #include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>
// #include "servo.h"
// #include "leds.h"
// #include "display.h"
// #include "waterpump.h"
// #include "mqtt_client.h"

// #ifdef __AVR__
// #include <util/delay.h>
// #include <avr/interrupt.h>
// #endif

// static uint8_t _buff[100];
// static uint8_t _index = 0;
// volatile static bool _done = false;

// // UART receive interrupt callback
// void console_rx(uint8_t _rx)
// {
//     uart_send_blocking(USART_0, _rx);

//     if (_rx != '\r' && _rx != '\n')
//     {
//         if (_index < sizeof(_buff) - 1)
//         {
//             _buff[_index++] = _rx;
//         }
//     }
//     else
//     {
//         _buff[_index] = '\0';
//         _index = 0;
//         _done = true;

//         uart_send_blocking(USART_0, '\n');
//         uart_send_string_blocking(USART_0, "Received: ");
//         uart_send_string_blocking(USART_0, (char *)_buff);
//         uart_send_blocking(USART_0, '\n');
//     }
// }

// int main()
// {
//     char welcome_text[] = "Welcome from SEP4 IoT hardware!\n";
//     char prompt_text[] = "Type text to send: ";

//     uart_init(USART_0, 9600, console_rx);
//     wifi_init();

// #ifdef __AVR__
//     sei();  // Enable interrupts
// #endif

//     uart_send_string_blocking(USART_0, welcome_text);

//     // Connect to Wi-Fi
//     wifi_command_join_AP("ONEPLUS", "00000000");
//     uart_send_string_blocking(USART_0, "Wi-Fi Connected\n");

//     // Connect to MQTT broker
//     wifi_command_create_TCP_connection("broker.hivemq.com", 1883, NULL, NULL);
//     uart_send_string_blocking(USART_0, "TCP Connected to MQTT Broker\n");

//     // Connect to frontend/backend server
//     wifi_command_create_TCP_connection("172.20.80.1", 23, NULL, NULL);
//     uart_send_string_blocking(USART_0, "TCP Connected to Frontend Backend\n");

//     // Send MQTT CONNECT packet
//     mqtt_connect("greenhouse_device_01");
//     uart_send_string_blocking(USART_0, "MQTT CONNECT Sent\n");

//     uart_send_string_blocking(USART_0, prompt_text);

//     // Initialize hardware
//     leds_init();
//     display_init();
//     display_int(123);  // ← Add this line
//     waterpump_init();

//     while (1)
//     {
//         if (_done)
//         {
//             mqtt_publish("greenhouse/sensor/temp", (char *)_buff);
//             uart_send_string_blocking(USART_0, "MQTT PUBLISH Sent\n");

//             if (strcmp((char *)_buff, "BUTTON1") == 0)
//             {
//                 uart_send_string_blocking(USART_0, "Button 1 triggered!\n");
//                 servo(0);
//                 leds_turnOn(1);
//                 display_int(1);
//             }
//             else if (strcmp((char *)_buff, "BUTTON2") == 0)
//             {
//                 uart_send_string_blocking(USART_0, "Button 2 triggered!\n");
//                 servo(90);
//                 leds_turnOn(2);
//                 display_int(2);
//             }
//             else if (strcmp((char *)_buff, "BUTTON3") == 0)
//             {
//                 uart_send_string_blocking(USART_0, "Button 3 triggered!\n");
//                 servo(180);
//                 leds_turnOn(3);
//                 display_int(3);
//             }
//             else if (strcmp((char *)_buff, "LED4_ON") == 0)
//             {
//                 leds_turnOn(4);
//             }
//             else if (strcmp((char *)_buff, "LED4_OFF") == 0)
//             {
//                 leds_turnOff(4);
//             }
//             else if (strcmp((char *)_buff, "PUMP_ON") == 0)
//             {
//                 waterpump_start();
//             }
//             else if (strcmp((char *)_buff, "PUMP_OFF") == 0)
//             {
//                 waterpump_stop();
//             }
//             else
//             {
//                 // Try converting to number
//                 int value = atoi((char *)_buff);
//                 if (value != 0 || strcmp((char *)_buff, "0") == 0)
//                 {
//                     display_int(value);
//                     uart_send_string_blocking(USART_0, "Displayed on 7-segment\n");
//                 }
//                 else
//                 {
//                     // Unknown command, fallback to TCP transmit
//                     wifi_command_TCP_transmit(_buff, strlen((char *)_buff));
//                     uart_send_string_blocking(USART_0, "Unknown command, sent over TCP\n");
//                 }
//             }

//             memset(_buff, 0, sizeof(_buff));
//             _done = false;
//             uart_send_string_blocking(USART_0, prompt_text);
//         }
//     }

//     return 0;
// }

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

#ifdef __AVR__
#include <util/delay.h>
#include <avr/interrupt.h>
#endif

#define MAX_STRING_LENGTH 100

static uint8_t uart_buffer[MAX_STRING_LENGTH] = {0};
static uint8_t uart_index = 0;
static volatile bool uart_done = false;

static char tcp_rx_buffer[MAX_STRING_LENGTH] = {0};
static bool tcp_string_received = false;

// --------------------------------------------------
// Utilities
// --------------------------------------------------

static void strip_newline(char *str) {
    for (size_t i = 0; str[i]; i++) {
        if (str[i] == '\r' || str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
}

// --------------------------------------------------
// UART receive callback
// --------------------------------------------------

void console_rx(uint8_t byte) {
    uart_send_blocking(USART_0, byte);  // Echo

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

// --------------------------------------------------
// TCP receive callback
// --------------------------------------------------

void tcp_rx(void) {
    size_t len = strlen(tcp_rx_buffer);
    tcp_rx_buffer[len] = '\r';
    tcp_rx_buffer[len + 1] = '\n';
    tcp_rx_buffer[len + 2] = '\0';
    tcp_string_received = true;
}

// --------------------------------------------------
// Command handler
// --------------------------------------------------

static void handle_command(const char *cmd) {
    strip_newline((char *)cmd);

    uart_send_string_blocking(USART_0, "DEBUG: Received raw command: [");
    uart_send_string_blocking(USART_0, cmd);
    uart_send_string_blocking(USART_0, "]\n");

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
    } else if (strcmp(cmd, "PUMP_ON_5S") == 0) {
        control_waterpump_on();
#ifdef __AVR__
        _delay_ms(5000);
#endif
        control_waterpump_off();
        mqtt_publish("greenhouse/status/pump", "ON_5S");
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

// --------------------------------------------------
// Main
// --------------------------------------------------

int main(void) {
    uart_init(USART_0, 9600, console_rx);
    wifi_init();
#ifdef __AVR__
    sei();
#endif

    uart_send_string_blocking(USART_0, "Welcome from SEP4 IoT hardware!\n");

    wifi_command_join_AP("ONEPLUS", "00000000");
    uart_send_string_blocking(USART_0, "Wi-Fi Connected\n");

    wifi_command_create_TCP_connection("192.168.219.114", 5000, tcp_rx, (uint8_t *)tcp_rx_buffer);
    uart_send_string_blocking(USART_0, "TCP Connected to Frontend Backend\n");

    //MQTT connection request
    mqtt_connect("greenhouse_device_01");
    uart_send_string_blocking(USART_0, "MQTT CONNECT Sent\n");

    uart_send_string_blocking(USART_0, "Type text to send: ");

    control_leds_init();
    control_display_init();
    control_waterpump_init();
    control_display_set_number(0);

    uint32_t loop_counter = 0;

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

        if (++loop_counter >= 5000) {
            loop_counter = 0;
            int temp = 200 + (rand() % 100);  // 20.0°C to 29.9°C
            char temp_msg[32];
            sprintf(temp_msg, "TEMP: %d.%d\n", temp / 10, temp % 10);
            uart_send_string_blocking(USART_0, temp_msg);
            wifi_command_TCP_transmit((uint8_t *)temp_msg, strlen(temp_msg));
        }

#ifdef __AVR__
        _delay_ms(1);
#endif
    }

    return 0;
}
