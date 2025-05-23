#include "proximity_controller.h"
#include "hc_sr04.h"

void control_proximity_init(void) {
    hc_sr04_init();
}

uint16_t control_proximity_get_distance_cm(void) {
    return hc_sr04_getDistance();
}

uint8_t control_proximity_is_close(uint16_t threshold_cm) {
    uint16_t distance = control_proximity_get_distance_cm();
    return (distance > 0 && distance <= threshold_cm) ? 1 : 0;
}