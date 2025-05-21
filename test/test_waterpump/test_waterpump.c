#include <unity.h>
#include "waterpump.h"
#include "waterpump_controller.h"

extern uint8_t PORTL;
extern uint8_t DDRL;
#define PL6 6

void setUp(void) {
    DDRL = 0;
    PORTL = 0;
}

void tearDown(void) {}

void test_waterpump_init_should_set_pl6_output_and_off(void) {
    waterpump_init();
    TEST_ASSERT_EQUAL_HEX8((1 << PL6), DDRL);    // PL6 set as output
    TEST_ASSERT_EQUAL_HEX8(0x00, PORTL);         // PL6 low (pump off)
}

void test_waterpump_start_should_set_pl6_high(void) {
    waterpump_init();
    waterpump_start();
    TEST_ASSERT_TRUE(PORTL & (1 << PL6));        // PL6 high (relay ON)
}

void test_waterpump_stop_should_clear_pl6_low(void) {
    waterpump_init();
    waterpump_start();
    waterpump_stop();
    TEST_ASSERT_FALSE(PORTL & (1 << PL6));       // PL6 low (relay OFF)
}

void test_waterpump_run_5s_should_start_and_stop(void) {
    waterpump_init();
    control_waterpump_run_5s();
    TEST_ASSERT_FALSE(PORTL & (1 << PL6));       // Should be OFF at the end
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_waterpump_init_should_set_pl6_output_and_off);
    RUN_TEST(test_waterpump_start_should_set_pl6_high);
    RUN_TEST(test_waterpump_stop_should_clear_pl6_low);
    RUN_TEST(test_waterpump_run_5s_should_start_and_stop);  // NEW TEST
    return UNITY_END();
}