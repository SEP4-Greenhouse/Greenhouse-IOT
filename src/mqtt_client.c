#include "mqtt_client.h"
#include "wifi.h"
#include "waterpump_controller.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>


// MQTT packet types (control packet type + flags)
#define MQTT_PACKET_CONNECT    0x10  // Connect packet type
#define MQTT_PACKET_PUBLISH    0x30  // Publish packet type
#define MQTT_PACKET_SUBSCRIBE  0x82  // Subscribe packet type

#define MAX_MQTT_RX_SIZE 128
char mqtt_rx_buffer[MAX_MQTT_RX_SIZE]; // Global RX receive buffer for incoming MQTT data

/**
 * @brief Sends an MQTT CONNECT packet to the broker
 * 
 * @param client_id The unique identifier for the client connecting to the broker
 */
 
void mqtt_connect(const char* client_id) {
    uint8_t packet[128];  // Buffer to build the MQTT connect packet
    size_t index = 0;

    packet[index++] = MQTT_PACKET_CONNECT;

    // Calculate remaining length (variable header + payload)
    size_t client_len = strlen(client_id);
    uint8_t remaining_length = 10 + 2 + client_len;  // 10 for protocol info, 2 for client ID length
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
    packet[index++] = (client_len >> 8) & 0xFF;     // Client ID length MSB
    packet[index++] = client_len & 0xFF;            // Client ID length LSB
    memcpy(&packet[index], client_id, client_len);  // Copy client ID to packet
    index += client_len;

    // Send the packet via WiFi (ESP8266)
    wifi_command_TCP_transmit(packet, index);
}

/**
 * @brief Publishes a message to the given MQTT topic (Each piece of hardware counts as a separate topic)
 * 
 * @param topic The MQTT topic to publish to
 * @param message The message payload to send
 */

 void mqtt_publish(const char* topic, const char* message) {
    uint8_t packet[256]; // Buffer for building the MQTT publis packet
    size_t index = 0;

    // Length calculations
    size_t topic_len = strlen(topic);
    size_t message_len = strlen(message);
    size_t remaining_length = 2 + topic_len + message_len;  // 2 bytes for topic length


    packet[index++] = MQTT_PACKET_PUBLISH; // MQTT PUBLISH packet type
    packet[index++] = remaining_length;    // Remaining lengths

    packet[index++] = (topic_len >> 8) & 0xFF;  // Topic length MSB
    packet[index++] = topic_len & 0xFF;         // Topic length LSB
    memcpy(&packet[index], topic, topic_len);   // Copy topic name
    index += topic_len;

    // Message payload
    memcpy(&packet[index], message, message_len);
    index += message_len;

    // Send packet over TCP via WiFi
    wifi_command_TCP_transmit(packet, index);
}

/**
 * @brief Subscribes to a given MQTT topic with QoS 0
 * 
 * @param topic The topic to subscribe to
 */

void mqtt_subscribe(const char* topic) {
    uint8_t packet[128];                              // Buffer to build the MQTT subscribe packet
    size_t index = 0;

    uint16_t packet_id = 1;                           // Packet identifier
    size_t topic_len = strlen(topic);
    size_t remaining_length = 2 + 2 + topic_len + 1;  // 2 bytes for packet ID, 2 bytes for topic length, 1 byte for QoS


    packet[index++] = MQTT_PACKET_SUBSCRIBE;   // MQTT SUBSCRIBE packet type
    packet[index++] = remaining_length;        // Remaining length

    packet[index++] = (packet_id >> 8) & 0xFF;    // Packet ID MSB (most significant bytes)
    packet[index++] = packet_id & 0xFF;           // Packet ID LSB (less significant bytes)

    packet[index++] = (topic_len >> 8) & 0xFF;   // Topic length MSB
    packet[index++] = topic_len & 0xFF;          // Topic length LSB

    //copies the MQTT topic string (like "greenhouse/temp") into the packet buffer, starting at the current index
    memcpy(&packet[index], topic, topic_len); 
    index += topic_len;

    // Requested QoS  (quality of service)
    // = 0 (at most once delivery)
    // = 1 (at least once delivery)
    // = 2 (exactly once delivery)

    packet[index++] = 0x00;
    
    
    // Send packet to broker via WiFi
    wifi_command_TCP_transmit(packet, index);
}