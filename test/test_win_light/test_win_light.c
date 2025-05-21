#include "../fff.h"
#include "unity.h"
#include "light.h"

#ifndef WINDOWS_TEST
#include <util/delay.h>
#endif

#include <stdio.h>
#include <stdint.h>

// Simulated AVR registers (mocked for Windows test)
uint8_t ADMUX;
uint8_t ADCSRA;
uint8_t ADCSRB;
uint8_t ADCL;
uint8_t ADCH;
uint8_t DIDR2;
uint8_t PORTK;
uint8_t DDRK;

DEFINE_FFF_GLOBALS;

void setUp(void) {
    // Clear registers before each test if needed
    ADMUX = 0;
    ADCSRA = 0;
    ADCSRB = 0;
    ADCL = 0;
    ADCH = 0;
    DIDR2 = 0;
}

void tearDown(void) {
    // Optional cleanup after each test
}

void test_light_init_sets_expected_registers(void) {
    light_init();

    // Check that the correct bits are set
    TEST_ASSERT_EQUAL_HEX8(0x40, ADMUX);   // REFS0 = 1
    TEST_ASSERT_EQUAL_HEX8(0x87, ADCSRA);  // ADEN, ADPS2, ADPS1, ADPS0
    TEST_ASSERT_EQUAL_HEX8(0x80, DIDR2);   // ADC15D = 1
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_light_init_sets_expected_registers);
    return UNITY_END();
}
