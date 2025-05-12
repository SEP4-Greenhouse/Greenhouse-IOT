#include "leds_controller.h"
#include "leds.h" // Use the provided low-level driver

void control_leds_init(void)
{
    leds_init();
}

void control_led_on(uint8_t led_no)
{
    leds_turnOn(led_no);
}

void control_led_off(uint8_t led_no)
{
    leds_turnOff(led_no);
}

void control_led_toggle(uint8_t led_no)
{
    leds_toggle(led_no);
}
