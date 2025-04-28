#include "mqtt_client.h"
#include "wifi.h"
#include <string.h>

// MQTT fixed values
#define MQTT_PACKET_CONNECT 0x10
#define MQTT_PACKET_PUBLISH 0x30

void mqtt_connect(const char* client_id) {
    uint8_t packet[128];
    size_t index = 0;

    // Fixed header
    packet[index++] = MQTT_PACKET_CONNECT;

    // Remaining length (variable header + payload)
    // Variable header = 10 bytes, payload = 2 + strlen(client_id)
    size_t client_len = strlen(client_id);
    uint8_t remaining_length = 10 + 2 + client_len;
    packet[index++] = remaining_length;

    // Protocol Name "MQTT"
    packet[index++] = 0x00;
    packet[index++] = 0x04;
    packet[index++] = 'M';
    packet[index++] = 'Q';
    packet[index++] = 'T';
    packet[index++] = 'T';

    // Protocol Level
    packet[index++] = 0x04; // MQTT 3.1.1

    // Connect Flags: clean session
    packet[index++] = 0x02;

    // Keep alive (in seconds)
    packet[index++] = 0x00;
    packet[index++] = 60;

    // Payload: Client ID
    packet[index++] = (client_len >> 8) & 0xFF;
    packet[index++] = client_len & 0xFF;
    memcpy(&packet[index], client_id, client_len);
    index += client_len;

    // Send the packet
    wifi_command_TCP_transmit(packet, index);
}

void mqtt_publish(const char* topic, const char* message) {
    uint8_t packet[256];
    size_t index = 0;

    size_t topic_len = strlen(topic);
    size_t message_len = strlen(message);
    size_t remaining_length = 2 + topic_len + message_len;

    // Fixed header
    packet[index++] = MQTT_PACKET_PUBLISH;
    packet[index++] = remaining_length;

    // Topic
    packet[index++] = (topic_len >> 8) & 0xFF;
    packet[index++] = topic_len & 0xFF;
    memcpy(&packet[index], topic, topic_len);
    index += topic_len;

    // Payload
    memcpy(&packet[index], message, message_len);
    index += message_len;

    // Send the packet
    wifi_command_TCP_transmit(packet, index);
}
