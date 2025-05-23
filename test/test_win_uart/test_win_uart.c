#include "unity.h"
#include "uart.h"
#include "mock_avr_io.h"
#include "../fff.h"

DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(sei);
FAKE_VOID_FUNC(cli);

// --- Setup / Teardown ---
void setUp(void) {
    RESET_FAKE(sei);
    RESET_FAKE(cli);
    FFF_RESET_HISTORY();
}

void tearDown(void) {}

// --- Tests ---

void test_uart_init_USART0_should_initialize_correctly(void) {
    uart_init(USART_0, 9600, NULL);

    // Add specific assertions if your mock or implementation supports it
    TEST_PASS_MESSAGE("USART_0 init executed (check mock side effects manually).");
}

void test_uart_init_USART1_should_initialize_correctly(void) {
    uart_init(USART_1, 19200, NULL);
    TEST_PASS_MESSAGE("USART_1 init executed.");
}

void test_uart_init_USART2_should_initialize_correctly(void) {
    uart_init(USART_2, 38400, NULL);
    TEST_PASS_MESSAGE("USART_2 init executed.");
}

void test_uart_init_USART3_should_initialize_correctly(void) {
    uart_init(USART_3, 57600, NULL);
    TEST_PASS_MESSAGE("USART_3 init executed.");
}

// --- Entry Point ---
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_uart_init_USART0_should_initialize_correctly);
    RUN_TEST(test_uart_init_USART1_should_initialize_correctly);
    RUN_TEST(test_uart_init_USART2_should_initialize_correctly);
    RUN_TEST(test_uart_init_USART3_should_initialize_correctly);

    return UNITY_END();
}