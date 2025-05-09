#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

typedef enum {
    DHT11_OK = 0,
    DHT11_TIMEOUT,
    DHT11_CHECKSUM_ERROR,
    DHT11_FAIL
} DHT11_ERROR_MESSAGE_t;

/**
 * @brief Initializes the DHT11 sensor (optional).
 */
void dht11_init(void);

/**
 * @brief Reads temperature and humidity values from the DHT11 sensor.
 *
 * @param[out] humidity_integer Integer part of humidity
 * @param[out] humidity_decimal Decimal part of humidity (always 0 for DHT11)
 * @param[out] temperature_integer Integer part of temperature
 * @param[out] temperature_decimal Decimal part of temperature (always 0 for DHT11)
 * @return DHT11_OK, DHT11_TIMEOUT, or DHT11_CHECKSUM_ERROR
 */
DHT11_ERROR_MESSAGE_t dht11_get(uint8_t* humidity_integer,
                                 uint8_t* humidity_decimal,
                                 uint8_t* temperature_integer,
                                 uint8_t* temperature_decimal);

#endif // DHT11_H