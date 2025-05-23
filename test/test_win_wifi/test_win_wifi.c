#include "fff.h"
#include "unity.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "wifi_mock.h"

#ifdef WINDOWS_TEST
#include "uart_mock.h"
#else
#include "uart.h"
#endif

DEFINE_FFF_GLOBALS;

// Fake system functions
FAKE_VOID_FUNC(sei);
FAKE_VOID_FUNC(cli);
FAKE_VOID_FUNC(_delay_ms, int);

// Fake UART functions
FAKE_VOID_FUNC(uart_send_string_blocking, USART_t, const char *);
FAKE_VOID_FUNC(uart_init, USART_t, uint32_t, UART_Callback_t);
FAKE_VOID_FUNC(uart_send_array_blocking, USART_t, uint8_t *, uint16_t);
FAKE_VOID_FUNC(uart_send_array_nonBlocking, USART_t, uint8_t *, uint16_t);
FAKE_VALUE_FUNC(UART_Callback_t, uart_get_rx_callback, USART_t);

// Fake application callbacks
FAKE_VOID_FUNC(TCP_Received_callback_func);

// External real callback (defined in application code)
extern void tcp_rx(void);

uint8_t TEST_BUFFER[128];

void setUp(void) {
    RESET_FAKE(uart_init);
    RESET_FAKE(uart_send_string_blocking);
    RESET_FAKE(uart_send_array_blocking);
    RESET_FAKE(TCP_Received_callback_func);
    wifi_init();  // Initializes the mock UART state
}

void tearDown(void) {}

WIFI_ERROR_MESSAGE_t run_and_feed_uart(WIFI_ERROR_MESSAGE_t (*wifi_fn)(), const char *response) {
    simulate_uart_receive(response);  // Feed the simulated UART response
    return wifi_fn();                // Now run the command to parse
}

WIFI_ERROR_MESSAGE_t run_join_ap(void) {
    return wifi_command_join_AP("ssid", "pass");
}

WIFI_ERROR_MESSAGE_t run_tcp_conn(void) {
    return wifi_command_create_TCP_connection("192.168.0.1", 8000, TCP_Received_callback_func, TEST_BUFFER);
}

// --- AT Command Tests ---

void test_wifi_command_AT_sends_correct_stuff_to_uart(void) {
    wifi_command_AT();
    TEST_ASSERT_EQUAL(USART_2, uart_send_string_blocking_fake.arg0_val);
    TEST_ASSERT_EQUAL_STRING("AT\r\n", uart_send_string_blocking_fake.arg1_val);
}

void test_wifi_command_AT_OK(void) {
    TEST_ASSERT_EQUAL(WIFI_OK, run_and_feed_uart(wifi_command_AT, "OK\r\n"));
}

void test_wifi_command_AT_ERROR(void) {
    TEST_ASSERT_EQUAL(WIFI_ERROR_RECEIVED_ERROR, run_and_feed_uart(wifi_command_AT, "ERROR"));
}

void test_wifi_command_AT_GARBAGE(void) {
    TEST_ASSERT_EQUAL(WIFI_ERROR_RECEIVING_GARBAGE, run_and_feed_uart(wifi_command_AT, "ER\0OR"));
}

void test_wifi_command_AT_NOTHING(void) {
    TEST_ASSERT_EQUAL(WIFI_ERROR_NOT_RECEIVING, wifi_command_AT());
}

// --- Join AP Tests ---

void test_wifi_join_AP_OK(void) {
    TEST_ASSERT_EQUAL(WIFI_OK, run_and_feed_uart(run_join_ap, "OK\r\n"));
}

void test_wifi_join_AP_FAIL(void) {
    TEST_ASSERT_EQUAL(WIFI_FAIL, run_and_feed_uart(run_join_ap, "FAIL\r\n"));
}

// --- TCP Connection Tests ---

void test_wifi_TCP_connection_OK(void) {
    TEST_ASSERT_EQUAL(WIFI_OK, run_and_feed_uart(run_tcp_conn, "OK\r\n"));
}

void test_wifi_TCP_connection_FAIL(void) {
    TEST_ASSERT_EQUAL(WIFI_FAIL, run_and_feed_uart(run_tcp_conn, "FAIL\r\n"));
}

// --- TCP Receive Test ---

void test_wifi_TCP_receive_triggers_callback(void) {
    run_and_feed_uart(run_tcp_conn, "OK\r\n");

    const char *data = "\r\n+IPD,4:ABCD";
    simulate_uart_receive(data);  // Simulate the full IPD message

    TEST_ASSERT_EQUAL_STRING("ABCD", TEST_BUFFER);
    TEST_ASSERT_EQUAL(1, TCP_Received_callback_func_fake.call_count);
}

// --- Main Entry Point ---

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_command_AT_sends_correct_stuff_to_uart);
    RUN_TEST(test_wifi_command_AT_OK);
    RUN_TEST(test_wifi_command_AT_ERROR);
    RUN_TEST(test_wifi_command_AT_GARBAGE);
    RUN_TEST(test_wifi_command_AT_NOTHING);
    RUN_TEST(test_wifi_join_AP_OK);
    RUN_TEST(test_wifi_join_AP_FAIL);
    RUN_TEST(test_wifi_TCP_connection_OK);
    RUN_TEST(test_wifi_TCP_connection_FAIL);
    RUN_TEST(test_wifi_TCP_receive_triggers_callback);
    return UNITY_END();
}
