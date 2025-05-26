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

#include "uart.h"
#include "wifi.h"
#include "mqtt_client.h"

#include "button_controller.h"
#include "pir_controller.h"
#include "proximity_controller.h"
#include "servo_controller.h"
#include "leds_controller.h"
#include "7segment_controller.h"
#include "waterpump_controller.h"
#include "dht11.h"

#ifdef __AVR__
#include <util/delay.h>
#include <avr/interrupt.h>
#endif

#define MAX_STRING_LENGTH 100
#define READ_INTERVAL_MS 15000  // 15 seconds
#define LOOP_DELAY_MS 10

static uint8_t uart_buffer[MAX_STRING_LENGTH] = {0};
static uint8_t uart_index = 0;
static volatile bool uart_done = false;

static char tcp_rx_buffer[MAX_STRING_LENGTH] = {0};
static bool tcp_string_received = false;

static int last_valid_display = 234; // Start with dummy 23.4°C

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
    uart_send_blocking(USART_0, byte);  // Echo back
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

void handle_command(const char *cmd) {
    strip_newline((char *)cmd); // Remove any trailing newline characters from the input command
    uart_send_string_blocking(USART_0, "DEBUG: Received command: ");
    uart_send_string_blocking(USART_0, cmd);
    uart_send_blocking(USART_0, '\n');

    // Control LEDs based on BUTTON commands
    if (strcmp(cmd, "LED4_ON") == 0) {
        control_led_on(4);
    } else if (strcmp(cmd, "LED4_OFF") == 0) {
        control_led_off(4);

    // Control water pump and publish MQTT status
    } else if (strcmp(cmd, "PUMP_ON") == 0) {
        control_waterpump_on();
        mqtt_publish("greenhouse/status/pump", "ON");
    } else if (strcmp(cmd, "PUMP_OFF") == 0) {
        control_waterpump_off();
        mqtt_publish("greenhouse/status/pump", "OFF");

#ifdef __AVR__
        _delay_ms(5000);
#endif
        control_waterpump_off();
        mqtt_publish("greenhouse/status/pump", "ON_5S");

    // If the input is a number, display it; otherwise, forward as TCP
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
    uart_init(USART_0, 9600, console_rx);      // Initialize UART at 9600 baud with RX interrupt
    wifi_init();                               // Initialize Wi-Fi module
    dht11_init();                              // Initialize DHT11 sensor

    uint8_t fan_state = 0;
    uint8_t pump_state = 0;

    control_buttons_init();

#ifdef __AVR__
    sei(); // Enable global interrupts on AVR
#endif

    // Initial system status
    uart_send_string_blocking(USART_0, "Welcome from SEP4 IoT hardware!\n");

    // Connect to Wi-Fi network
    wifi_command_join_AP("ONEPLUS", "00000000");
    uart_send_string_blocking(USART_0, "Wi-Fi Connected\n");

    // Connect to TCP server (e.g., frontend/backend)
    wifi_command_create_TCP_connection("192.168.6.114", 5000, tcp_rx, (uint8_t *)tcp_rx_buffer);
    uart_send_string_blocking(USART_0, "TCP Connected to Frontend Backend\n");

    // Connect to MQTT broker
    mqtt_connect("greenhouse_device_01");
    uart_send_string_blocking(USART_0, "MQTT CONNECT Sent\n");

    // Display prompt
    uart_send_string_blocking(USART_0, "Type text to send: ");

    // Initialize hardware components
    control_leds_init();
    control_display_init();
    control_waterpump_init(); // Uncomment if pump is used
    control_servo_init();
    control_pir_init();
    control_proximity_init();
    control_buttons_init();

    control_display_set_number(last_valid_display);  // Show dummy temp initially (23.4°C)

    // Variables for periodic reading
    uint32_t loop_counter = 0;
    uint32_t press_duration = 0;
    uint8_t is_long_pressing = 0;
    uint8_t last_direction = 1;  // 1 = clockwise, 0 = counter
    const uint32_t loop_threshold = READ_INTERVAL_MS / LOOP_DELAY_MS;

    while (1) {
        // Check if UART input is ready
        if (uart_done) {
            mqtt_publish("greenhouse/sensor/temp", (char *)uart_buffer); // Publish received command
            handle_command((char *)uart_buffer);                          // Handle command
            uart_done = false;
            memset(uart_buffer, 0, sizeof(uart_buffer));                  // Clear buffer
            uart_send_string_blocking(USART_0, "Type text to send: ");

            // Handle specific fan commands
            if (strcmp((char *)uart_buffer, "servo_up") == 0) {
                control_servo_step_clockwise();
            } else if (strcmp((char *)uart_buffer, "servo_down") == 0) {
                control_servo_step_counterclockwise();
            } else if (strcmp((char *)uart_buffer, "servo_status") == 0) {
                const char* status = control_servo_get_position_status();
                uart_send_string_blocking(USART_0, status);
                uart_send_string_blocking(USART_0, "\n");
            } else {
                uart_send_string_blocking(USART_0, "Unknown command.\n");
            }

            uart_done = false;
            memset(uart_buffer, 0, sizeof(uart_buffer));
            uart_send_string_blocking(USART_0, "Awaiting next input: ");
        }

        // Check for TCP input
        if (tcp_string_received) {
            uart_send_string_blocking(USART_0, "TCP Received: ");
            uart_send_string_blocking(USART_0, tcp_rx_buffer);
            handle_command(tcp_rx_buffer);           // Process TCP command
            tcp_string_received = false;
        }

        // Periodic temperature reading block
        if (++loop_counter >= loop_threshold) {
            loop_counter = 0;
            _delay_ms(100); // Small wait before reading

            uint8_t temp_int = 0, temp_dec = 0;
            bool success = false;

            // Try reading DHT11 sensor up to 3 times
            for (int i = 0; i < 3; i++) {
                if (dht11_get(NULL, NULL, &temp_int, &temp_dec) == DHT11_OK) {
                    success = true;
                    break;
                }
                _delay_ms(200); // Delay between retries
            }

            if (control_pir_is_motion_detected()) {
                uart_send_string_blocking(USART_0, "[PIR] Motion detected\n");
            }

            // If reading successful, display and publish
            if (success) {
                last_valid_display = temp_int * 10 + (temp_dec % 10);  // Combine int and 1 decimal digit

                char msg[64];
                sprintf(msg, "TEMP: %d.%dC\n", temp_int, temp_dec);
                uart_send_string_blocking(USART_0, msg);
                wifi_command_TCP_transmit((uint8_t *)msg, strlen(msg));

                control_display_set_number(last_valid_display); // Update 7-segment display
            } else {
                uart_send_string_blocking(USART_0, "DHT11 Read FAIL\n"); // Reading failed, retain last value
            }

            // Proximity Check
            uint16_t distance = control_proximity_get_distance_cm();
            char dist_msg[40];
            sprintf(dist_msg, "[PROXIMITY] Distance: %u cm\n", distance);
            uart_send_string_blocking(USART_0, dist_msg);

            if (distance <= 15) {
                uart_send_string_blocking(USART_0, "[Proximity] Object detected up close\n");
            }
        }

        // Handle button presses (Toggle Servo Motor angle, Toggle Water Pump ON/OFF, Show sensor status via UART, & Perform a "Soft Reset")
        if (control_button_s1_pressed()) {
            press_duration++;
            if (press_duration > 2000 && !is_long_pressing) {
                is_long_pressing = 1;
            }
            if (is_long_pressing) {
                if (last_direction) {
                    if (control_servo_get_angle() >= 90) last_direction = 0;
                } else {
                    if (control_servo_get_angle() <= 0) last_direction = 1;
                }

                if (last_direction) control_servo_step_clockwise();
                else control_servo_step_counterclockwise();
                _delay_ms(150);
            }
        } else {
            if (press_duration > 0 && press_duration <= 2000) {
                uint8_t angle = control_servo_get_angle();
                if (angle == 90) control_servo_set_angle(0);
                else control_servo_set_angle(90);
            }
            press_duration = 0;
            is_long_pressing = 0;
        }

        if (control_button_s2_pressed()) {
            if (!pump_state) {
                control_waterpump_on();
                pump_state = 1;
            } else {
                control_waterpump_off();
                pump_state = 0;
            }
            _delay_ms(250);
        }

        if (control_button_s3_pressed()) {
            // Sensor snapshot
            uint8_t motion = control_pir_is_motion_detected();
            uint16_t dist = control_proximity_get_distance_cm();
            char buffer[64];
            sprintf(buffer, "Status - Motion: %s | Distance: %u cm\n", motion ? "YES" : "NO", dist);
            uart_send_string_blocking(USART_0, buffer);
            _delay_ms(250);
        }

        if (control_button_s4_pressed()) {
            // Soft reset (turn everything off)
            control_servo_set_angle(0);        // Close the window
            control_waterpump_off();           // Turn off the pump
            uart_send_string_blocking(USART_0, "System: soft reset (all off)\n");
            _delay_ms(500);  // Debounce delay
        }

        _delay_ms(LOOP_DELAY_MS); // Main loop pacing
    }

    return 0;
}