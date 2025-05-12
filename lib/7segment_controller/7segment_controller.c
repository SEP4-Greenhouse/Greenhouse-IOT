#include "7segment_controller.h"
#include "display.h" // ✅ Use the provided low-level driver (display.c)

void control_display_init(void)
{
    display_init();
}

void control_display_set_number(int16_t number)
{
    display_int(number);
}

void control_display_custom_segments(uint8_t seg1, uint8_t seg2, uint8_t seg3, uint8_t seg4)
{
    display_setValues(seg1, seg2, seg3, seg4);
}