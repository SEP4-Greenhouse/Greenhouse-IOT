#include "waterpump_controller.h"
#include "waterpump.h"
#include "soil.h"  // Assumes there's a soil moisture sensor driver

void pump_controller_update(void) {
    if (soil_is_dry() && !pump_is_running()) {
        pump_run(3000); // Run pump for 3 seconds
    }
}