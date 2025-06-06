#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


// Include headers for various device drivers and controllers
#include "wifi.h"
#include "uart.h"
#include "leds_controller.h"
#include "7segment_controller.h"
#include "waterpump_controller.h"
#include "mqtt_client.h"
#include "dht11.h"
#include "moisture_controller.h"
#include "pir_controller.h"
#include "proximity_controller.h"

#ifdef __AVR__
#include <util/delay.h>
#include <avr/interrupt.h>
#endif


// Constants for buffer sizes and timing intervals
#define MAX_STRING_LENGTH 100
#define READ_INTERVAL_MS 15000
#define LOOP_DELAY_MS 10


// UART input buffer
static uint8_t uart_buffer[MAX_STRING_LENGTH] = {0};
static uint8_t uart_index = 0;
static volatile bool uart_done = false;

// Initial values shown on the display before actual reading data is displayed
static int last_valid_display = 234;

// MQTT receive buffer and status flag
static char mqtt_rx_buffer[256] = {0};
static bool mqtt_response_received = false;


/**
 * @brief Callback when MQTT message is received
 * Adds newline and prints it to UART for logging.
 */
void mqtt_rx(void) {
    size_t len = strlen(mqtt_rx_buffer);
    mqtt_rx_buffer[len] = '\r';
    mqtt_rx_buffer[len + 1] = '\n';
    mqtt_rx_buffer[len + 2] = '\0';


    /**
    * @brief Generates a fake ISO-8601 timestamp for MQTT messages
    */
    uart_send_string_blocking(USART_0, "[MQTT RX] Response:\n");
    uart_send_string_blocking(USART_0, mqtt_rx_buffer);
    mqtt_response_received = true;
}

char* get_fake_timestamp(void) {
    static char buffer[40];
    static unsigned long total_seconds = 0;

    total_seconds += 15;
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
     * @brief Utility to strip newline characters from command strings
     */
void strip_newline(char *str) {
    for (size_t i = 0; str[i]; i++) {
        if (str[i] == '\r' || str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
}
    /**
     * @brief UART receive handler for command input
     */
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



    // Handling the LED and display commands
    if (strcmp(cmd, "BUTTON1") == 0) {
        control_led_on(1);
        control_display_set_number(1);
    } else if (strcmp(cmd, "BUTTON2") == 0) {
        control_led_on(2);
        control_display_set_number(2);
    } else if (strcmp(cmd, "BUTTON3") == 0) {
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
        
    } else if (strcmp(cmd, "LED_ALL_OFF") == 0) {
        for (int i = 1; i <= 4; i++) control_led_off(i);

    } else if (strcmp(cmd, "DISPLAY_RESET") == 0) {
        control_display_set_number(0);



    // Sensor read commands
    } else if (strcmp(cmd, "READ_DHT11") == 0) {
        uint8_t hum_int, hum_dec, temp_int, temp_dec;
        if (dht11_get(&hum_int, &hum_dec, &temp_int, &temp_dec) == DHT11_OK) {
            char buf[64];
            snprintf(buf, sizeof(buf), "Temp: %dC, Humidity: %d%%\n", temp_int, hum_int);
            uart_send_string_blocking(USART_0, buf);
        } else {
            uart_send_string_blocking(USART_0, "Failed to read DHT11 sensor.\n");
        }

    } else if (strcmp(cmd, "READ_MOISTURE") == 0) {
        uint8_t moisture = control_moisture_get_percent();
        char buf[40];
        snprintf(buf, sizeof(buf), "Soil moisture: %d%%\n", moisture);
        uart_send_string_blocking(USART_0, buf);

    } else if (strcmp(cmd, "READ_PROXIMITY") == 0) {
        uint16_t distance = control_proximity_get_distance_cm();
        char buf[40];
        snprintf(buf, sizeof(buf), "Proximity: %d cm\n", distance);
        uart_send_string_blocking(USART_0, buf);

    } else if (strcmp(cmd, "READ_PIR") == 0) {
        bool motion = control_pir_detected();
        uart_send_string_blocking(USART_0, motion ? "Motion detected\n" : "No motion\n");

    } else {
        int value = atoi(cmd);
        if (value || strcmp(cmd, "0") == 0) {
            control_display_set_number(value);
        } else {
            uart_send_string_blocking(USART_0, "Unknown command, echoed\n");
        }
    }
}

/**
 * @brief Application entry point — system setup and main control loop
 */
int main(void) {
    uart_init(USART_0, 9600, console_rx);
    wifi_init();
    dht11_init();
    _delay_ms(2000);  // Letting sensors settle and read
    control_moisture_init();
    control_pir_init();
    control_proximity_init();

#ifdef __AVR__
    sei();   // Enable global interrupts
#endif

    uart_send_string_blocking(USART_0, "Welcome from SEP4 IoT hardware!\n");
    wifi_command_join_AP("ONEPLUS", "00000000");
    uart_send_string_blocking(USART_0, "Wi-Fi Connected\n");

    // MQTT onnection to broker
    wifi_command_create_TCP_connection("broker.hivemq.com", 1883, mqtt_rx, (uint8_t*)mqtt_rx_buffer);
    uart_send_string_blocking(USART_0, "TCP Connected to MQTT Broker\n");

    mqtt_connect("greenhouse_device_01");
    uart_send_string_blocking(USART_0, "MQTT CONNECT Sent\n");

    // MQTT subscribe to backend commands
    mqtt_subscribe("greenhouse/control/pump");
    uart_send_string_blocking(USART_0, "MQTT SUBSCRIBE Sent\n");

    uart_send_string_blocking(USART_0, "Type text to send: ");

    // Initialization of actuators and display
    control_leds_init();
    control_display_init();
    control_waterpump_init();
    control_display_set_number(last_valid_display);

    uint32_t loop_counter = 0;
    const uint32_t loop_threshold = READ_INTERVAL_MS / LOOP_DELAY_MS;

    while (1) {
        // Handle UART input and commands
        if (uart_done) {
            mqtt_publish("greenhouse/sensor/temp", (char *)uart_buffer);
            handle_command((char *)uart_buffer);
            uart_done = false;
            memset(uart_buffer, 0, sizeof(uart_buffer));
            uart_send_string_blocking(USART_0, "Type text to send: ");
        }


        // Periodic sensor data read and MQTT publish
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

                // Update display and publish to MQTT
                last_valid_display = temp_int * 10 + (temp_dec % 10);
                control_display_set_number(last_valid_display);

                char* timestamp = get_fake_timestamp();
                char mqtt_msg[128];

                snprintf(mqtt_msg, sizeof(mqtt_msg),
                         "{ \"timeStamp\": \"%s\", \"value\": %d, \"unit\": \"\xC2\xB0C\", \"sensorId\": 1 }",
                         timestamp, temp_int);
                mqtt_publish("greenhouse/sensor/temperature", mqtt_msg);

                snprintf(mqtt_msg, sizeof(mqtt_msg),
                         "{ \"timeStamp\": \"%s\", \"value\": %d, \"unit\": \"%%\", \"sensorId\": 2 }",
                         timestamp, hum_int);
                mqtt_publish("greenhouse/sensor/humidity", mqtt_msg);

                uint8_t moisture_percent = control_moisture_get_percent();
                snprintf(mqtt_msg, sizeof(mqtt_msg),
                         "{ \"timeStamp\": \"%s\", \"value\": %d, \"unit\": \"%%\", \"sensorId\": 3 }",
                         timestamp, moisture_percent);
                mqtt_publish("greenhouse/sensor/moisture", mqtt_msg);
            } else {
                uart_send_string_blocking(USART_0, "DHT11 Read FAIL\n");
            }

            // Read and report proximity
            uint16_t distance = control_proximity_get_distance_cm();
            char dist_msg[40];
            snprintf(dist_msg, sizeof(dist_msg), "[PROXIMITY] Distance: %u cm\n", distance);
            uart_send_string_blocking(USART_0, dist_msg);

            if (distance <= 15) {
                uart_send_string_blocking(USART_0, "[Proximity] Object detected up close\n");
            }
        }

        _delay_ms(LOOP_DELAY_MS);  // Adding delay for the loop to avoid sensor reading delays which are built in
    }

    return 0;
}
