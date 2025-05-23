#include "servo_controller.h"
#include "servo.h" // Include your existing driver

void control_servo_motor(uint8_t angle)
{
    servo(angle);
}