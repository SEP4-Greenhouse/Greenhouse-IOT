#include <unity.h>
#include "waterpump.h"

extern uint8_t PORTC;
extern uint8_t DDRC;
#define PC7 7

void setUp(void) {
    DDRC = 0;
    PORTC = 0;
}

void tearDown(void) {}

void test_waterpump_init_should_set_pc7_output_and_off(void) {
    waterpump_init();
    TEST_ASSERT_EQUAL_HEX8((1 << PC7), DDRC);    // PC7 set as output
    TEST_ASSERT_EQUAL_HEX8(0x00, PORTC);         // PC7 low (pump off)
}

void test_waterpump_start_should_set_pc7_high(void) {
    waterpump_init();
    waterpump_start();
    TEST_ASSERT_TRUE(PORTC & (1 << PC7));        // PC7 high (relay ON)
}

void test_waterpump_stop_should_clear_pc7_low(void) {
    waterpump_init();
    waterpump_start();
    waterpump_stop();
    TEST_ASSERT_FALSE(PORTC & (1 << PC7));       // PC7 low (relay OFF)
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_waterpump_init_should_set_pc7_output_and_off);
    RUN_TEST(test_waterpump_start_should_set_pc7_high);
    RUN_TEST(test_waterpump_stop_should_clear_pc7_low);
    return UNITY_END();
}
