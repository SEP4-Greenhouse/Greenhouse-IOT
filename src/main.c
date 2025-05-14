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

#include <util/delay.h>
#include <avr/interrupt.h>

#define MAX_STRING_LENGTH 100

static uint8_t _buff[100];
static uint8_t _index = 0;
volatile static bool _done = false;

static uint8_t _console_receive_buff[MAX_STRING_LENGTH] = {0};
static bool _console_string_received = false;
static char _tcp_receive_buff[MAX_STRING_LENGTH] = {0};
static bool _tcp_string_received = false;

// This is a callback function. Execution time must be short!

// Helper to trim CR/LF
void strip_newline(char* str) {
    for (size_t i = 0; str[i]; i++) {
        if (str[i] == '\r' || str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
}

// This is a callback function. Execution time must be short!
void console_rx(uint8_t _rx)
{
    uart_send_blocking(USART_0, _rx);
    uart_send_blocking(USART_0, _rx);   // Echo (for demo purposes)
    if(('\r' != _rx) && ('\n' != _rx))
    {
        if(_index < 100-1)
        {
            _buff[_index++] = _rx;
            _console_receive_buff[_index++] = _rx;
        }
    }
    else
    {
        _buff[_index] = '\0';
        _console_receive_buff[_index] = '\0';
        _index = 0;
        _done = true;
        uart_send_blocking(USART_0, '\n');
//        uart_send_string_blocking(USART_0, (char*)_buff);
        _console_string_received = true;
        uart_send_blocking(USART_0, '\n');   // Echo (for demo purposes)
    }
}

// This is a callback function. Execution time must be short!
void tcp_rx()
{
    uint8_t index = strlen(_tcp_receive_buff);
    _tcp_receive_buff[index] = '\r';
    _tcp_receive_buff[index+1] = '\n';
    _tcp_receive_buff[index+2] = '\0';
    _tcp_string_received = true;
}

void handle_command(char* cmd)
{
    strip_newline(cmd);

    uart_send_string_blocking(USART_0, "DEBUG: Received raw command: [");
    uart_send_string_blocking(USART_0, cmd);
    uart_send_string_blocking(USART_0, "]\n");

    uart_send_string_blocking(USART_0, "Handling command: ");
    uart_send_string_blocking(USART_0, cmd);
    uart_send_blocking(USART_0, '\n');

    if (strcmp(cmd, "BUTTON1") == 0) {
        control_servo_motor(0);
        control_led_on(1);
        control_display_set_number(1);
        uart_send_string_blocking(USART_0, "BUTTON1 OK\n");
    } else if (strcmp(cmd, "BUTTON2") == 0) {
        control_servo_motor(90);
        control_led_on(2);
        control_display_set_number(2);
        uart_send_string_blocking(USART_0, "BUTTON2 OK\n");
    } else if (strcmp(cmd, "BUTTON3") == 0) {
        control_servo_motor(180);
        control_led_on(3);
        control_display_set_number(3);
        uart_send_string_blocking(USART_0, "BUTTON3 OK\n");
    } else if (strcmp(cmd, "LED4_ON") == 0) {
        control_led_on(4);
        uart_send_string_blocking(USART_0, "LED4 ON\n");
    } else if (strcmp(cmd, "LED4_OFF") == 0) {
        control_led_off(4);
        uart_send_string_blocking(USART_0, "LED4 OFF\n");
    } else if (strcmp(cmd, "PUMP_ON") == 0) {
        control_waterpump_on();
        mqtt_publish("greenhouse/status/pump", "ON");
        uart_send_string_blocking(USART_0, "Pump ON\n");
    } else if (strcmp(cmd, "PUMP_OFF") == 0) {
        control_waterpump_off();
        mqtt_publish("greenhouse/status/pump", "OFF");
        uart_send_string_blocking(USART_0, "Pump OFF\n");
    } else if (strcmp(cmd, "PUMP_ON_5S") == 0) {
        control_waterpump_on();
        uart_send_string_blocking(USART_0, "Pump ON for 5s\n");
#ifdef __AVR__
        _delay_ms(5000);
#endif
        control_waterpump_off();
        uart_send_string_blocking(USART_0, "Pump OFF after 5s\n");
        mqtt_publish("greenhouse/status/pump", "ON_5S");
    } else {
        int value = atoi(cmd);
        if (value != 0 || strcmp(cmd, "0") == 0) {
            control_display_set_number(value);
            uart_send_string_blocking(USART_0, "Number displayed\n");
        } else {
            wifi_command_TCP_transmit((uint8_t*)cmd, strlen(cmd));
            uart_send_string_blocking(USART_0, "Unknown command, echoed\n");
        }
    }
}


int main()
{
    char welcome_text[] = "Welcome from SEP4 IoT hardware!\n";
    char prompt_text[] = "Type text to send: ";

    uart_init(USART_0, 9600, console_rx);
    wifi_init();
    sei();

    uart_send_string_blocking(USART_0, welcome_text);
    wifi_command_join_AP("ONEPLUS", "00000000");
    uart_send_string_blocking(USART_0, "Wi-Fi Connected\n");

    wifi_command_create_TCP_connection("192.168.219.114", 5000, tcp_rx, _tcp_receive_buff);
    uart_send_string_blocking(USART_0, "TCP Connected to Frontend Backend\n");

    mqtt_connect("greenhouse_device_01");
    uart_send_string_blocking(USART_0, "MQTT CONNECT Sent\n");

    uart_send_string_blocking(USART_0, prompt_text);

    control_leds_init();
    control_display_init();
    control_waterpump_init();
    control_display_set_number(0);

    uint32_t counter = 0;

    while (1)
    {
        if (_done)
        {
        
            mqtt_publish("greenhouse/sensor/temp", (char *)_buff);
            uart_send_string_blocking(USART_0, "MQTT PUBLISH Sent\n");

            handle_command((char*)_buff);

            memset(_buff, 0, sizeof(_buff));
            _done = false;
            uart_send_string_blocking(USART_0, prompt_text);
        }
        
        if (_tcp_string_received)
        {
            uart_send_string_blocking(USART_0, "TCP Received: ");
            uart_send_string_blocking(USART_0, _tcp_receive_buff);
            handle_command(_tcp_receive_buff);  // Optional: Treat it as a command
            _tcp_string_received = false;
        }

        if (++counter >= 5000)
        {
            counter = 0;
            int temp = 200 + (rand() % 100);
            char temp_str[32];
            sprintf(temp_str, "TEMP: %d.%d\n", temp / 10, temp % 10);
            uart_send_string_blocking(USART_0, temp_str);

            uart_send_string_blocking(USART_0, _tcp_receive_buff);
            _tcp_string_received = false; // new

            wifi_command_TCP_transmit((uint8_t*)temp_str, strlen(temp_str));

        }

        _delay_ms(1);
    }

    return 0;
}
