#include "waterpump_controller.h"
#include "waterpump.h"

void control_waterpump_init(void)
{
    waterpump_init();
}

void control_waterpump_on(void)
{
    waterpump_start();
}

void control_waterpump_off(void)
{
    waterpump_stop();
}
