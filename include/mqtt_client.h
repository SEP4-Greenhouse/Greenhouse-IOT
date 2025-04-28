#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <stdint.h>
#include <stddef.h>

// Call this to send CONNECT packet
void mqtt_connect(const char* client_id);

// Call this to send PUBLISH packet
void mqtt_publish(const char* topic, const char* message);

#endif // MQTT_CLIENT_H
