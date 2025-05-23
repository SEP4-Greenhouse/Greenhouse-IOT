#include "../fff.h"
#include "unity.h"
#include "pc_comm.h"
#include "uart.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

DEFINE_FFF_GLOBALS;

// --- Fakes ---
FAKE_VOID_FUNC(uart_init, USART_t, uint32_t, UART_Callback_t);
FAKE_VOID_FUNC(uart_send_array_blocking, USART_t, uint8_t*, uint16_t);
FAKE_VOID_FUNC(uart_send_string_blocking, USART_t, char*);
FAKE_VOID_FUNC(uart_send_array_nonBlocking, USART_t, uint8_t*, uint16_t);

// --- Setup / Teardown ---
void setUp(void)
{
    RESET_FAKE(uart_init);
    RESET_FAKE(uart_send_array_blocking);
    RESET_FAKE(uart_send_string_blocking);
    RESET_FAKE(uart_send_array_nonBlocking);
    FFF_RESET_HISTORY();

    pc_comm_init(115200, NULL);
}

void tearDown(void) {}

// --- Tests ---
void test_pc_comm_default_callback_func_is_null(void)
{
    TEST_ASSERT_NULL(uart_init_fake.arg2_val);
    TEST_ASSERT_EQUAL(1, uart_init_fake.call_count);
}

void test_pc_comm_baudrates(void)
{
    TEST_ASSERT_EQUAL(115200, uart_init_fake.arg1_val);
    pc_comm_init(9600, NULL);
    TEST_ASSERT_EQUAL(9600, uart_init_fake.arg1_val);
}

void test_pc_comm_send_blocking(void)
{
    char data[] = "hejsa";
    pc_comm_send_array_blocking((uint8_t *) data, 4);

    TEST_ASSERT_EQUAL_PTR(data, uart_send_array_blocking_fake.arg1_val);
    TEST_ASSERT_EQUAL(4, uart_send_array_blocking_fake.arg2_val);
}

void test_pc_comm_string_blocking(void)
{
    char data[] = "hejsa";
    pc_comm_send_string_blocking(data);

    TEST_ASSERT_EQUAL_STRING(data, uart_send_string_blocking_fake.arg1_val);
}

// --- Optional: Visual spacer for output ---
char message_string[1024];
void message(void)
{
    for (int i = 0; i < 300; i++) UNITY_OUTPUT_CHAR(' ');
    for (int i = 0; i < strlen(message_string); i++) UNITY_OUTPUT_CHAR(message_string[i]);
    for (int i = 0; i < 300; i++) UNITY_OUTPUT_CHAR(' ');
}

// --- Entry Point ---
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_pc_comm_default_callback_func_is_null);
    RUN_TEST(test_pc_comm_baudrates);
    RUN_TEST(test_pc_comm_send_blocking);

    TEST_MESSAGE("m e s s a g e :1:_:PASS\n");

    RUN_TEST(test_pc_comm_string_blocking);
    return UNITY_END();
}
