#include "pir_controller.h"
#include "pir.h"
#include "uart.h"
#include <stdbool.h>

// Internal callback function for when motion is detected
static void motion_detected_callback(void) {
    uart_send_string_blocking(USART_0, "PIR: Motion detected!\n");
}

// Initialize PIR sensor and attach motion callback
void control_pir_init(void) {
    pir_init(motion_detected_callback);
}

// Disable PIR sensor
void control_pir_deinit(void) {
    pir_deinit();
}

// Query whether motion has been detected
uint8_t control_pir_is_motion_detected(void) {
    return pir_is_motion_detected();  // 1 = motion, 0 = no motion
}

bool control_pir_detected() {
    // Replace with actual PIR sensor logic
    return false;
}