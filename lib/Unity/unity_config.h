#include <unity.h>

void test_dummy(void) {
    TEST_ASSERT_EQUAL_INT(1, 1);
}

void setUp(void) {}     // optiona;
void tearDown(void) {}  // optional

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_dummy);
    return UNITY_END();
}
