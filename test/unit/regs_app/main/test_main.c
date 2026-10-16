#include <stdlib.h>
#include "unity.h"

void setUp(void) {
}

void tearDown(void) {
}

void app_main(void) {
   UNITY_BEGIN();
   unity_run_all_tests();
   exit(UNITY_END());
}
