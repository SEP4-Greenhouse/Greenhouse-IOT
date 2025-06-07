#include "waterpump_controller.h"  // Header for this module's function declarations
#include "waterpump.h"             // Lower-level water pump driver (actual hardware logic)
#include "uart.h"                  // For sending debug/status messages over UART

#ifdef __AVR__
#include <util/delay.h>           // AVR-specific delay functions (_delay_ms), only included if compiling for AVR
#endif

/**
 * @brief Initializes the water pump system.
 * 
 * Calls the lower-level pump_init function and sends a confirmation over UART.
 */
void control_waterpump_init(void)
{
    pump_init();  // Setup hardware needed to control the pump
    uart_send_string_blocking(USART_0, "Water pump initialized\n");  // Inform host system
}

/**
 * @brief Starts the water pump if not already running.
 * 
 * Uses pump_start to engage the pump. Sends a message depending on result.
 */
void control_waterpump_on(void)
{
    if (pump_start()) {
        uart_send_string_blocking(USART_0, "Pump started\n");  // Successfully started
    } else {
        uart_send_string_blocking(USART_0, "Pump already running\n");  // Already active
    }
}

/**
 * @brief Stops the water pump unconditionally.
 */
void control_waterpump_off(void)
{
    pump_stop();  // Deactivate pump
    uart_send_string_blocking(USART_0, "Pump stopped\n");  // Inform system on IoT side 
}

/**
 * @brief Runs the pump for exactly 5 seconds.
 * 
 * Starts the pump, waits 5 seconds (blocking), then stops it.
 * Note: Works only on AVR-based platforms due to _delay_ms usage.
 */
void control_waterpump_run_5s(void)
{
    uart_send_string_blocking(USART_0, "Pump running for 5s...\n");
    pump_start();  // Start pump

#ifdef __AVR__
    _delay_ms(5000);  // Wait 5000ms (5 seconds)
#endif

    pump_stop();  // Stop pump
    uart_send_string_blocking(USART_0, "Pump run complete\n");  // Notify
}
