#include "leds_controller.h"
#include "leds.h" // Use the provided low-level driver


// try with mocking
// Initialize the LED subsystem
void control_leds_init(void)
{
    leds_init(); // Setup GPIOs and turn off all LEDs initially
}

// Turn on a specific LED by index
void control_led_on(uint8_t led_no)
{
    leds_turnOn(led_no); // Activates specified LED
}

// Turn off a specific LED by index
void control_led_off(uint8_t led_no)
{
    leds_turnOff(led_no); // Deactivates specified LED
}

// Toggle a specific LED on/off
void control_led_toggle(uint8_t led_no)
{
    leds_toggle(led_no); // Inverts current state of LED
}
