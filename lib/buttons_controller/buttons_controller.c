#include "buttons_controller.h"
#include "buttons.h" // Use the teacher's button driver

void control_buttons_init(void)
{
    buttons_init();
}

uint8_t control_is_button1_pressed(void)
{
    return buttons_1_pressed();
}

uint8_t control_is_button2_pressed(void)
{
    return buttons_2_pressed();
}

uint8_t control_is_button3_pressed(void)
{
    return buttons_3_pressed();
}