#include "wifi.h"
#include "uart.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "servo.h"        // Correct: servo driver
#include "leds.h"         // Correct: leds driver
#include "display.h"      // Correct: 7segment display driver
#include "waterpump.h"   // Correct: water pump driver
#include "mqtt_client.h"

#ifdef __AVR__
#include <util/delay.h>
#include <avr/interrupt.h>
#endif

static uint8_t _buff[100];
static uint8_t _index = 0;
volatile static bool _done = false;
void console_rx(uint8_t _rx)
{
    uart_send_blocking(USART_0, _rx);
    if(('\r' != _rx) && ('\n' != _rx))
    {
        if(_index < 100-1)
        {
            _buff[_index++] = _rx;
        }
    }
    else
    {
        _buff[_index] = '\0';
        _index = 0;
        _done = true;
        uart_send_blocking(USART_0, '\n');
        uart_send_string_blocking(USART_0, (char*)_buff);
    }
}

int main()
{
    char welcome_text[] = "Welcome from SEP4 IoT hardware!\n";
 //   char hardware_msg[] = "Initialized hardware peripherals!\n";
    char prompt_text[] = "Type text to send: ";


    uart_init(USART_0, 9600, console_rx);
    wifi_init();

    #ifdef __AVR__
    sei();
    #endif

    // Step 1: Connect to Wi-Fi
    wifi_command_join_AP("ONEPLUS", "00000000");
    uart_send_string_blocking(USART_0, "Wi-Fi Connected\n");

    // Step 2: Connect to TCP or MQTT Broker
    // Connect to both if needed, or conditionally based on use case
    wifi_command_create_TCP_connection("broker.hivemq.com", 1883, NULL, NULL); // MQTT Broker
    uart_send_string_blocking(USART_0, "TCP Connected to MQTT Broker\n");

    // Optionally connect to frontend backend server (for UI commands)
     wifi_command_create_TCP_connection("172.20.80.1", 23, NULL, NULL);
     uart_send_string_blocking(USART_0, "TCP Connected to Frontend Backend\n");

    // Step 3: Send MQTT CONNECT packet
    mqtt_connect("greenhouse_device_01");
    uart_send_string_blocking(USART_0, "MQTT CONNECT Sent\n");

   // uart_send_string_blocking(USART_0, hardware_msg);
    uart_send_string_blocking(USART_0, prompt_text);

    // Initialize hardware peripherals
    leds_init();
    display_init();
    waterpump_init();
    //uart_send_string_blocking(USART_0, hardware_msg);

    
    // Step 4: Loop
    while (1)
    {
        if (_done)
        {
            // MQTT Publish simulated sensor data (for example purposes)
            mqtt_publish("greenhouse/sensor/temp", (char*)_buff);
            uart_send_string_blocking(USART_0, "MQTT PUBLISH Sent\n");

            // Handle frontend commands
            if (strcmp((char*)_buff, "BUTTON1") == 0)
            {
                uart_send_string_blocking(USART_0, "Button 1 triggered from Frontend!\n");
                servo(0);
                leds_turnOn(1);
                display_int(1);
            }
            else if (strcmp((char*)_buff, "BUTTON2") == 0)
            {
                uart_send_string_blocking(USART_0, "Button 2 triggered from Frontend!\n");
                servo(90);
                leds_turnOn(2);
                display_int(2);
            }
            else if (strcmp((char*)_buff, "BUTTON3") == 0)
            {
                uart_send_string_blocking(USART_0, "Button 3 triggered from Frontend!\n");
                servo(180);
                leds_turnOn(3);
                display_int(3);
            }
            else if (strcmp((char*)_buff, "LED4_ON") == 0)
            {
                leds_turnOn(4);
            }
            else if (strcmp((char*)_buff, "LED4_OFF") == 0)
            {
                leds_turnOff(4);
            }
            else if (strcmp((char*)_buff, "PUMP_ON") == 0)
            {
                waterpump_start();
            }
            else if (strcmp((char*)_buff, "PUMP_OFF") == 0)
            {
                waterpump_stop();
            }
            else
            {
                // Unknown command; send it via TCP
                wifi_command_TCP_transmit(_buff, strlen((char*)_buff));
            }

            _done = false;
            uart_send_string_blocking(USART_0, prompt_text);
        }
    }

    return 0;
}
