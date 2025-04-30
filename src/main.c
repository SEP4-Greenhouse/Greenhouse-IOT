#include "wifi.h"
#include "uart.h"
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#include "servo.h"        // Correct: servo driver
#include "leds.h"         // Correct: leds driver
#include "display.h"      // Correct: 7segment display driver
#include "waterpump.h"   // Correct: water pump driver

static uint8_t _buff[100];
static uint8_t _index = 0;
volatile static bool _done = false;

void console_rx(uint8_t _rx)
{
    uart_send_blocking(USART_0, _rx);
    if(('\r'!= _rx) && ('\n' != _rx))
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
    }
}

int main()
{
    char welcome_text[] = "Welcome from SEP4 IoT hardware!\n";
    char prompt_text[] = "Type text to send: ";

    uart_init(USART_0, 9600, console_rx);
    wifi_init();
    sei();

    wifi_command_join_AP("JESUS CHRIST", "Nya199200");
    wifi_command_create_TCP_connection("172.20.10.3", 23, NULL, NULL);
    wifi_command_TCP_transmit((uint8_t*)welcome_text, strlen((char*)welcome_text));
    uart_send_string_blocking(USART_0, prompt_text);

    leds_init();           // Correct: initialize LED driver
    display_init();        // Correct: initialize 7-segment display
    waterpump_init();      // Correct: initialize Water Pump driver

    while(1)
    {
        if(_done)
        {
            _done = false;

            if (strcmp((char*)_buff, "BUTTON1") == 0)
            {
                uart_send_string_blocking(USART_0, "Button 1 triggered from Frontend!\n");
                servo(0);          // Move servo to 0 degrees
                leds_turnOn(1);    // Turn ON LED 1
                display_int(1);    // Display 1 on 7-segment
            }
            else if (strcmp((char*)_buff, "BUTTON2") == 0)
            {
                uart_send_string_blocking(USART_0, "Button 2 triggered from Frontend!\n");
                servo(90);         // Move servo to 90 degrees
                leds_turnOn(2);    // Turn ON LED 2
                display_int(2);    // Display 2 on 7-segment
            }
            else if (strcmp((char*)_buff, "BUTTON3") == 0)
            {
                uart_send_string_blocking(USART_0, "Button 3 triggered from Frontend!\n");
                servo(180);        // Move servo to 180 degrees
                leds_turnOn(3);    // Turn ON LED 3
                display_int(3);    // Display 3 on 7-segment
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
                wifi_command_TCP_transmit(_buff, strlen((char*)_buff));
                uart_send_string_blocking(USART_0, prompt_text);
            }
        }
    }

    return 0;
}
