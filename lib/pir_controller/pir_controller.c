#include "pir_controller.h"
#include "pir.h"
#include "uart.h"

static void motion_detected_callback(void) {
    uart_send_string_blocking(USART_0, "PIR: Motion detected!\n");
}

void control_pir_init(void) {
    pir_init(motion_detected_callback);
}

void control_pir_deinit(void) {
    pir_deinit();
}

uint8_t control_pir_is_motion_detected(void) {
    return pir_is_motion_detected();  // 1 = motion, 0 = no motion
}