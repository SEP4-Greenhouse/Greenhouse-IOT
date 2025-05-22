#include "waterpump_controller.h"
#include "waterpump.h"

#ifdef __AVR__
#include <util/delay.h>
#endif

void control_waterpump_init(void)
{
    pump_init();
}

void control_waterpump_on(void)
{
    pump_start();
}

void control_waterpump_off(void)
{
    pump_stop();
}

void control_waterpump_run_5s(void)
{
    pump_start();
#ifdef __AVR__
    _delay_ms(5000);
#endif
    pump_stop();
}
