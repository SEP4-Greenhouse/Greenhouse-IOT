#include "waterpump_controller.h"
#include "waterpump.h"

#ifdef __AVR__
#include <util/delay.h>
#endif

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

void control_waterpump_run_5s(void)
{
    waterpump_start();
#ifdef __AVR__
    _delay_ms(5000);
#endif
    waterpump_stop();
}
