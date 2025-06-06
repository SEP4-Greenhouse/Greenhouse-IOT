#include "waterpump_controller.h"
#include "waterpump.h"
#include "uart.h"  // <- Needed for uart_send_string_blocking()

#ifdef __AVR__
#include <util/delay.h>
#endif

// Initialize the water pump and print confirmation
void control_waterpump_init(void)
{
    pump_init();
    uart_send_string_blocking(USART_0, "Water pump initialized\n");
}

// Attempt to turn the water pump on and notify via UART
void control_waterpump_on(void)
{
    if (pump_start()) {
        uart_send_string_blocking(USART_0, "Pump started\n");
    } else {
        uart_send_string_blocking(USART_0, "Pump already running\n");
    }
}

// Turn off the pump if it's running
void control_waterpump_off(void)
{
    pump_stop();
    uart_send_string_blocking(USART_0, "Pump stopped\n");
}

// Turn off the pump 5 seonds after it has stated running
void control_waterpump_run_5s(void)
{
    uart_send_string_blocking(USART_0, "Pump running for 5s...\n");
    pump_start();
#ifdef __AVR__
    _delay_ms(5000);
#endif
    pump_stop();
    uart_send_string_blocking(USART_0, "Pump run complete\n");
}