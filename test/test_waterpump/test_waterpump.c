#include "unity.h"
#include "waterpump.h"

// Mock registers
volatile uint8_t PORTC = 0;
volatile uint8_t DDRC = 0;
volatile uint32_t pump_duration = 0;
volatile uint8_t pump_running = 0;
#define PC7 7

void setUp(void) {
    PORTC = 0;
    DDRC = 0;
    pump_stop();
}

void tearDown(void) {}

void test_pump_init_should_set_pin_and_stop_pump(void) {
    pump_init();
    TEST_ASSERT_TRUE(DDRC & (1 << PC7));
    TEST_ASSERT_FALSE(PORTC & (1 << PC7));
}

void test_pump_start_should_turn_on_pump(void) {
    pump_init();
    uint8_t result = pump_start();
    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_TRUE(PORTC & (1 << PC7));
    TEST_ASSERT_TRUE(pump_is_running());
}

void test_pump_stop_should_turn_off_pump(void) {
    pump_init();
    pump_start();
    pump_stop();
    TEST_ASSERT_FALSE(PORTC & (1 << PC7));
    TEST_ASSERT_FALSE(pump_is_running());
}

void test_pump_run_should_turn_on_and_timeout(void) {
    pump_init();
    uint8_t result = pump_run(5);
    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_TRUE(PORTC & (1 << PC7));
    TEST_ASSERT_TRUE(pump_is_running());

    // Simulate timer ISR manually
    for (int i = 0; i < 5; ++i) {
        if (pump_running && pump_duration > 0) {
            pump_duration--;
            if (pump_duration == 0) {
                pump_stop();
            }
        }
    }

    TEST_ASSERT_FALSE(pump_is_running());
    TEST_ASSERT_FALSE(PORTC & (1 << PC7));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_pump_init_should_set_pin_and_stop_pump);
    RUN_TEST(test_pump_start_should_turn_on_pump);
    RUN_TEST(test_pump_stop_should_turn_off_pump);
    RUN_TEST(test_pump_run_should_turn_on_and_timeout);
    return UNITY_END();
}