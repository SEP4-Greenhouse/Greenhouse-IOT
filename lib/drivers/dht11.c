/**
 * @file dht11.c
 * @brief Driver for DHT11 temperature and humidity sensor
 *
 * Communicates using the proprietary single-wire timing protocol.
 */

#include "dht11.h"
#include "includes.h"
#include <avr/interrupt.h>   // for cli(), sei()
#include <util/delay.h>

<<<<<<< HEAD
// === Pin Configuration ===
#define DHT_DDR     DDRL
#define DHT_PORT    PORTL
#define DHT_PIN     PINL
#define DHT_BIT     PL1

// === Timing & Parsing Constants ===
#define MAX_TIMINGS         85
#define TIMEOUT_COUNT       255
#define DHT_HIGH_THRESHOLD  26  // Bit value threshold

/**
 * @brief Initializes the DHT11 (optional if GPIO-controlled power is used).
 */
void dht11_init(void) {
    // Optional: configure GPIO for VCC/GND if needed
    // VCC_DDR |= (1 << VCC_BIT);
    // VCC_PORT |= (1 << VCC_BIT);
    //
    // GND_DDR |= (1 << GND_BIT);
    // GND_PORT &= ~(1 << GND_BIT);
}

/**
 * @brief Read temperature and humidity from the DHT11 sensor.
 *
 * @param[out] humidity_integer Integer part of humidity (%)
 * @param[out] humidity_decimal Decimal part of humidity (always 0 for DHT11)
 * @param[out] temperature_integer Integer part of temperature (°C)
 * @param[out] temperature_decimal Decimal part of temperature (always 0 for DHT11)
 * @return DHT11_OK if successful, DHT11_FAIL if timeout or checksum error
 */
DHT11_ERROR_MESSAGE_t dht11_get(uint8_t* humidity_integer, uint8_t* humidity_decimal,
                                 uint8_t* temperature_integer, uint8_t* temperature_decimal) {
    uint8_t data[5] = {0};
    uint8_t last_state = 1, counter = 0, bit_index = 0;

    // === Send Start Signal ===
    DHT_DDR |= (1 << DHT_BIT);        // Output
    DHT_PORT &= ~(1 << DHT_BIT);      // Pull LOW
    _delay_ms(18);                    // Minimum 18ms

    DHT_PORT |= (1 << DHT_BIT);       // Pull HIGH
    _delay_us(40);                    // 20–40us

    DHT_DDR &= ~(1 << DHT_BIT);       // Input
    DHT_PORT |= (1 << DHT_BIT);       // Pull-up enabled
    _delay_us(10);                    // Settle line

    // === Time-Critical Bit Read ===
    cli();  // Disable interrupts

    for (uint8_t i = 0; i < MAX_TIMINGS; i++) {
        counter = 0;
        while ((DHT_PIN & (1 << DHT_BIT)) == last_state) {
            counter++;
            _delay_us(1);
            if (counter == TIMEOUT_COUNT) {
                break;
            }
        }

        last_state = DHT_PIN & (1 << DHT_BIT);

        if (counter == TIMEOUT_COUNT)
            break;

        // Skip initial response transitions
        if ((i >= 4) && (i % 2 == 0)) {
            data[bit_index / 8] <<= 1;
            if (counter > DHT_HIGH_THRESHOLD)
                data[bit_index / 8] |= 1;
            bit_index++;
        }
    }

    sei();  // Re-enable interrupts

    // === Verify Checksum ===
    if ((bit_index >= 40) &&
        (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) {

        if (humidity_integer)     *humidity_integer     = data[0];
        if (humidity_decimal)     *humidity_decimal     = data[1];
        if (temperature_integer)  *temperature_integer  = data[2];
        if (temperature_decimal)  *temperature_decimal  = data[3];

        return DHT11_OK;
    } else {
        if (humidity_integer)     *humidity_integer     = 0;
        if (humidity_decimal)     *humidity_decimal     = 0;
        if (temperature_integer)  *temperature_integer  = 0;
        if (temperature_decimal)  *temperature_decimal  = 0;

        return DHT11_FAIL;
    }
}
=======
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
>>>>>>> 27b327cbae460f6565c7d1846198431da8aba846
