#include "7segment_controller.h"
#include "display.h" // ✅ Use the provided low-level driver (display.c)

// Initialize the 7-segment display system
void control_display_init(void)
{
    display_init(); // Call underlying hardware setup
}

// Display a signed integer value (e.g., sensor reading)
void control_display_set_number(int16_t value)
{
    display_int(value);  // // Shows integer value on 4-digit 7-segment display
}

// Manually set individual segment values for custom visuals
void control_display_custom_segments(uint8_t seg1, uint8_t seg2, uint8_t seg3, uint8_t seg4)
{
    display_setValues(seg1, seg2, seg3, seg4);
}