#include "dht11.h"
#include "includes.h"

// Define the data pin configuration
#define DATA_BIT PL1
#define DATA_PIN PINL
#define DATA_DDR DDRL
#define DATA_PORT PORTL

void dht11_init() {
    // Optionally, configure power pins here if necessary
    // Currently unused
}

#define MAX_TIMINGS 85

DHT11_ERROR_MESSAGE_t dht11_get(uint8_t* humidity_integer, uint8_t* humidity_decimal,
                                 uint8_t* temperature_integer, uint8_t* temperature_decimal) {
    uint8_t laststate = 1;
    uint8_t counter = 0;
    uint8_t j = 0;
    uint8_t i;
    uint8_t data[5] = {0};

    // Send start signal
    DATA_DDR |= (1 << DATA_BIT);   // Set pin as output
    DATA_PORT &= ~(1 << DATA_BIT); // Pull pin low
    _delay_ms(18);

    // Pull high for 20-40us
    DATA_PORT |= (1 << DATA_BIT);
    _delay_us(40);

    // Switch to input and enable pull-up
    DATA_DDR &= ~(1 << DATA_BIT);
    DATA_PORT |= (1 << DATA_BIT);

    // Read response and data
    for (i = 0; i < MAX_TIMINGS; i++) {
        counter = 0;
        while ((DATA_PIN & (1 << DATA_BIT)) == laststate) {
            counter++;
            _delay_us(1);
            if (counter == 255) {
                break;
            }
        }
        laststate = (DATA_PIN & (1 << DATA_BIT));

        if (counter == 255)
            break;

        if ((i >= 4) && (i % 2 == 0)) {
            data[j / 8] <<= 1;
            if (counter > 26)  // 1-bit threshold timing
                data[j / 8] |= 1;
            j++;
        }
    }

    // Check checksum
    if (j >= 40 &&
        data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {

        if (humidity_integer) *humidity_integer = data[0];
        if (humidity_decimal) *humidity_decimal = data[1];
        if (temperature_integer) *temperature_integer = data[2];
        if (temperature_decimal) *temperature_decimal = data[3];

        return DHT11_OK;
    } else {
        if (humidity_integer) *humidity_integer = 0;
        if (humidity_decimal) *humidity_decimal = 0;
        if (temperature_integer) *temperature_integer = 0;
        if (temperature_decimal) *temperature_decimal = 0;
        return DHT11_FAIL;
    }
}