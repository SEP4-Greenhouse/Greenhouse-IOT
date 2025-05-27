#pragma once

#include <stdint.h>

typedef struct {
    char topic[64];
    char payload[128];
} MQTTMessage;

MQTTMessage create_temperature_message(uint8_t temp_int, uint8_t temp_dec);
MQTTMessage create_humidity_message(uint8_t hum_int, uint8_t hum_dec);
MQTTMessage create_moisture_message(uint8_t moisture_percent);
MQTTMessage create_environment_report(uint8_t temp_int, uint8_t temp_dec, uint8_t hum_int, uint8_t hum_dec,uint8_t moisture_percent);
