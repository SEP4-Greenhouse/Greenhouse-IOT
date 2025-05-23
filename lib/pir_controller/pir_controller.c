#include "pir_controller.h"
#include "pir.h"

static void on_motion_detected_callback(void) {
    // Placeholder: You can trigger alerts, logs, or actuators here
}

void control_pir_init(void) {
    pir_init(on_motion_detected_callback);
}

void control_pir_deinit(void) {
    pir_deinit();
}

uint8_t control_pir_is_motion_detected(void) {
    return pir_is_motion_detected();
}