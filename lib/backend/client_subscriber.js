const mqtt = require('mqtt');

// Connect to the MQTT broker running locally
const client = mqtt.connect('mqtt://localhost');

client.on('connect', () => {
    console.log('👤 Client connected to MQTT broker');

    // Subscribe to the same topic as the backend
    client.subscribe('greenhouse/sensor/temp', (err) => {
        if (!err) {
            console.log('👤 Client subscribed to topic greenhouse/sensor/temp');
        }
    });
});

client.on('message', (topic, message) => {
    console.log(`📥 [Client] Received: ${message.toString()} on topic: ${topic}`);
});
