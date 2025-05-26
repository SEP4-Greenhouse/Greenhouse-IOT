// mqtt_message_factory.c
#include "mqtt_message_factory.h"
#include <stdio.h> // for snprintf

MQTTMessage create_temperature_message(uint8_t temp_int, uint8_t temp_dec) {
    MQTTMessage msg;
    snprintf(msg.topic, sizeof(msg.topic), "greenhouse/sensor/temperature");
    snprintf(msg.payload, sizeof(msg.payload), "%d.%d", temp_int, temp_dec);
    return msg;
}

MQTTMessage create_humidity_message(uint8_t hum_int, uint8_t hum_dec) {
    MQTTMessage msg;
    snprintf(msg.topic, sizeof(msg.topic), "greenhouse/sensor/humidity");
    snprintf(msg.payload, sizeof(msg.payload), "%d.%d", hum_int, hum_dec);
    return msg;
}

MQTTMessage create_moisture_message(uint8_t moisture_percent) {
    MQTTMessage msg;
    snprintf(msg.topic, sizeof(msg.topic), "greenhouse/sensor/moisture");
    snprintf(msg.payload, sizeof(msg.payload), "%u", moisture_percent);
    return msg;
}

MQTTMessage create_environment_report(uint8_t temp_int, uint8_t temp_dec,
                                      uint8_t hum_int, uint8_t hum_dec,
                                      uint8_t moisture_percent) {
    MQTTMessage msg;
    snprintf(msg.topic, sizeof(msg.topic), "greenhouse/sensor/environment");
    snprintf(msg.payload, sizeof(msg.payload),
             "{\"temperature\":\"%d.%d\",\"humidity\":\"%d.%d\",\"moisture\":\"%u\"}",
             temp_int, temp_dec, hum_int, hum_dec, moisture_percent);
    return msg;
}

