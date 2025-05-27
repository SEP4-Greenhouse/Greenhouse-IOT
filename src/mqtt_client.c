#include "mqtt_client.h"
#include "wifi.h"
#include "waterpump_controller.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>

#define MQTT_PACKET_CONNECT    0x10
#define MQTT_PACKET_PUBLISH    0x30
#define MQTT_PACKET_SUBSCRIBE  0x82

#define MAX_MQTT_RX_SIZE 128
char mqtt_rx_buffer[MAX_MQTT_RX_SIZE]; // Global RX buffer

void mqtt_connect(const char* client_id) {
    uint8_t packet[128];
    size_t index = 0;

    packet[index++] = MQTT_PACKET_CONNECT;

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

    packet[index++] = 0x04; // Protocol level 4 (MQTT 3.1.1)
    packet[index++] = 0x02; // Clean session
    packet[index++] = 0x00;
    packet[index++] = 60;   // Keep alive

    // Payload: client ID
    packet[index++] = (client_len >> 8) & 0xFF;
    packet[index++] = client_len & 0xFF;
    memcpy(&packet[index], client_id, client_len);
    index += client_len;

    wifi_command_TCP_transmit(packet, index);
}

void mqtt_publish(const char* topic, const char* message) {
    uint8_t packet[256];
    size_t index = 0;

    size_t topic_len = strlen(topic);
    size_t message_len = strlen(message);
    size_t remaining_length = 2 + topic_len + message_len;

    packet[index++] = MQTT_PACKET_PUBLISH;
    packet[index++] = remaining_length;

    packet[index++] = (topic_len >> 8) & 0xFF;
    packet[index++] = topic_len & 0xFF;
    memcpy(&packet[index], topic, topic_len);
    index += topic_len;

    memcpy(&packet[index], message, message_len);
    index += message_len;

    wifi_command_TCP_transmit(packet, index);
}

void mqtt_subscribe(const char* topic) {
    uint8_t packet[128];
    size_t index = 0;

    uint16_t packet_id = 1;
    size_t topic_len = strlen(topic);
    size_t remaining_length = 2 + 2 + topic_len + 1;

    packet[index++] = MQTT_PACKET_SUBSCRIBE;
    packet[index++] = remaining_length;

    packet[index++] = (packet_id >> 8) & 0xFF;
    packet[index++] = packet_id & 0xFF;

    packet[index++] = (topic_len >> 8) & 0xFF;
    packet[index++] = topic_len & 0xFF;
    memcpy(&packet[index], topic, topic_len);
    index += topic_len;

    packet[index++] = 0x00; // QoS 0

    wifi_command_TCP_transmit(packet, index);
}

// void mqtt_rx(void) {
//     uart_send_string_blocking(USART_0, "[MQTT RX] Message received:\n");
//     uart_send_string_blocking(USART_0, mqtt_rx_buffer);
//     uart_send_blocking(USART_0, '\n');

//     if (strstr(mqtt_rx_buffer, "greenhouse/control/pump")) {
//         if (strstr(mqtt_rx_buffer, "ON")) {
//             control_waterpump_on();
//             uart_send_string_blocking(USART_0, "[MQTT RX] Pump turned ON\n");
//         } else if (strstr(mqtt_rx_buffer, "OFF")) {
//             control_waterpump_off();
//             uart_send_string_blocking(USART_0, "[MQTT RX] Pump turned OFF\n");
//         } else {
//             uart_send_string_blocking(USART_0, "[MQTT RX] Unknown pump command\n");
//         }
//     }
// }
