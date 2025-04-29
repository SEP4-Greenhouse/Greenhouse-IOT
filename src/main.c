#include "wifi.h"
#include "uart.h"
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "mqtt_client.h" // need to implement or import a library

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
//        uart_send_string_blocking(USART_0, (char*)_buff);
    }
}

int main()
{
    char welcome_text[] = "Welcome from SEP4 IoT hardware!\n";
    char prompt_text[] = "Type text to send: ";

    uart_init(USART_0, 9600, console_rx);
    wifi_init();
    sei();

    // Step 1: Connect to Wi-Fi
    wifi_command_join_AP("JESUS CHRIST", "Nya199200");
    uart_send_string_blocking(USART_0, "Wi-Fi Connected\n");

    // Step 2: Connect to MQTT Broker
    wifi_command_create_TCP_connection("broker.hivemq.com", 1883, NULL, NULL); // This link needs to be changed to connect with Backend
    uart_send_string_blocking(USART_0, "TCP Connected\n");

    // Step 3: Send MQTT CONNECT packet
    mqtt_connect("greenhouse_device_01");
    uart_send_string_blocking(USART_0, "MQTT CONNECT Sent\n");

    uart_send_string_blocking(USART_0, prompt_text);

    // Step 4: Loop for publishing
    while (1)
    {
        if (_done)
        {
            mqtt_publish("greenhouse/sensor/temp", (char*)_buff);
            uart_send_string_blocking(USART_0, "MQTT PUBLISH Sent\n");
            _done = false;
            uart_send_string_blocking(USART_0, prompt_text);
        }
    }

    return 0;
}
