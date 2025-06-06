#include <stdio.h>
#include "proximity_controller.h"
#include "hc_sr04.h"
#include "uart.h"

#define DEFAULT_THRESHOLD_CM 20

// Initialize the proximity sensor (ultrasonic)
void control_proximity_init(void) {
    hc_sr04_init();
}

// Measure distance and log it via UART
uint16_t control_proximity_get_distance_cm(void) {
    uint16_t distance = hc_sr04_getDistance();  // Real reading from sensor
    char msg[40];
    sprintf(msg, "Proximity: %u cm\n", distance);
    uart_send_string_blocking(USART_0, msg);
    return distance;
}

// This function checks if the distance is within the threshold.
// It returns 1 if close, 0 otherwise.
uint8_t control_proximity_is_close(uint16_t threshold_cm) {
    if (threshold_cm == 0) threshold_cm = DEFAULT_THRESHOLD_CM;
    uint16_t dist = control_proximity_get_distance_cm();
    return (dist > 0 && dist <= threshold_cm);
}