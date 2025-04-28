const mqtt = require('mqtt');

// Connect to your local MQTT broker (Mosquitto in Docker)
const client = mqtt.connect('mqtt://host.docker.internal:1883');

client.on('connect', () => {
    console.log('✅ Backend connected to MQTT broker');
    
    // Subscribe to the Arduino topic
    client.subscribe('greenhouse/sensor/temp', (err) => {
        if (!err) {
            console.log('✅ Subscribed to greenhouse/sensor/temp');
        } else {
            console.error('❌ Subscribe error:', err);
        }
    });
});

client.on('message', (topic, message) => {
    console.log(`📥 New Message - Topic: ${topic} | Message: ${message.toString()}`);
});
