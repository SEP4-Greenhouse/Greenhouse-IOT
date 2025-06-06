#include "dht11.h"
#include "includes.h"

#ifdef __AVR__
#include "uart.h"
#include <avr/interrupt.h>
#endif

// DHT11 sensor is connected to Port L, Pin 1
#define DATA_BIT PL1
#define DATA_PIN PINL
#define DATA_DDR DDRL
#define DATA_PORT PORTL
#define READ_INTERVAL_MS 2000
#define MAX_TIMINGS 85

// Initialization function
void dht11_init() {
    // Optional hardware config
}

// Read temperature and humidity from DHT11 sensor
// Returns error code and fills in the passed humidity and temperature pointers
DHT11_ERROR_MESSAGE_t dht11_get(uint8_t* humidity_integer, uint8_t* humidity_decimal,
                                 uint8_t* temperature_integer, uint8_t* temperature_decimal) {
    uint8_t laststate = 1;
    uint8_t counter = 0;
    uint8_t j = 0;
    uint8_t i;
    uint8_t data[5] = {0};

     // Prepare to send start signal to sensor
    DATA_DDR |= (1 << DATA_BIT);     // Set pin as output
    DATA_PORT &= ~(1 << DATA_BIT);   // Pull pin low for 18 ms
    _delay_ms(18);
    DATA_PORT |= (1 << DATA_BIT);    // Pull pin high for 40 us
    _delay_us(40);
    DATA_DDR &= ~(1 << DATA_BIT);    // Set pin as input
    DATA_PORT |= (1 << DATA_BIT); // enable pull-up

#ifdef __AVR__
    cli();  // Disable interrupts
#endif

    // Read response and 40 bits of data (5 bytes)
    for (i = 0; i < MAX_TIMINGS; i++) {
        counter = 0;
        while ((DATA_PIN & (1 << DATA_BIT)) == laststate) {
            counter++;
            _delay_us(1);
            if (counter == 255) break;
        }
        laststate = (DATA_PIN & (1 << DATA_BIT));
        if (counter == 255) break;

        if ((i >= 4) && (i % 2 == 0)) {
            data[j / 8] <<= 1;
            if (counter > 26)
                data[j / 8] |= 1;
            j++;
        }
    }

#ifdef __AVR__
    sei();  // Re-enable interrupts
#endif

    // Debug print all received bits
#ifdef __AVR__
    char debug[64];
    sprintf(debug, "[DHT11] Bits read: %u\n", j);
    uart_send_string_blocking(USART_0, debug);
#endif

    // Check and debug results
    if (j >= 40 && data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
#ifdef __AVR__
        sprintf(debug, "[DHT11] Read OK: RH=%d.%d, T=%d.%d\n", data[0], data[1], data[2], data[3]);
        uart_send_string_blocking(USART_0, debug);
#endif
        if (humidity_integer) *humidity_integer = data[0];
        if (humidity_decimal) *humidity_decimal = data[1];
        if (temperature_integer) *temperature_integer = data[2];
        if (temperature_decimal) *temperature_decimal = data[3];

        return DHT11_OK;
    } else {
#ifdef __AVR__
        if (j < 40) {
            uart_send_string_blocking(USART_0, "[DHT11] FAIL: Not enough bits received\n");
        } else {
            uart_send_string_blocking(USART_0, "[DHT11] FAIL: Checksum mismatch\n");
        }
#endif
        if (humidity_integer) *humidity_integer = 0;
        if (humidity_decimal) *humidity_decimal = 0;
        if (temperature_integer) *temperature_integer = 0;
        if (temperature_decimal) *temperature_decimal = 0;
        return DHT11_FAIL;
    }
}