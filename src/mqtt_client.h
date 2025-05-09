#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

void mqtt_connect(const char* client_id);
void mqtt_publish(const char* topic, const char* message);

#endif // MQTT_CLIENT_H
