/**
 * @file user_tasks_init.c
 * @brief Create and start all user tasks.
 *        Called from main (wifi_slip_main.c) via extern declaration.
 */
#ifndef USER_TASKS_INIT_C
#define USER_TASKS_INIT_C 1
#include "main_config.h"
#include "common.h"
#include "link_functions.h"

/* Conditional includes – only pulled in when the file is compiled */
#if TOUCH_HANDLE_ENABLE
#include "touch_handle.h"
#endif
#if DI_HANDLING_ENABLE
#include "di_handle.h"
#endif
#if STEP_MOTOR
#include "step_motor.h"
#endif
#if FEEDER
#include "feeder.h"
#endif
#if POLISHER
#include "polisher.h"
#endif
#if TEST_INT
#include "test_int.h"
#endif
#if MORSE
#include "morse.h"
#endif
#if SCD41_ENABLE
#include "scd41.h"
#endif
#if EPAPER
#include "epaper.h"
#endif
#if PWM_CONTROL_ENABLE
#include "pwm_test.h"
#endif

static const char *TAG = "user_tasks_init";

void user_tasks_start(void *wakeup_cause) {
   BaseType_t res = pdTRUE;
   (void)res;

#if TOUCH_HANDLE_ENABLE
   res = link_functions.os_thread_create(touch_task, "touch_task", 4096, NULL, (tskIDLE_PRIORITY + 2), &touch_task_handle);
   if (res != pdTRUE) {
      link_functions.os_log_info(TAG, "create touch_task failed");
   }
#endif

#if DI_HANDLING_ENABLE
   res = link_functions.os_thread_create(di_handle_task, "di_handle_task", 2048, NULL, (tskIDLE_PRIORITY + 2), &di_handle_id);
   if (res != pdTRUE) {
      link_functions.os_log_info(TAG, "di_handle_task init failed");
   }
#endif

#if STEP_MOTOR
   res = link_functions.os_thread_create(step_motor_task, "step_motor_task", 2464, NULL, (tskIDLE_PRIORITY + 2),
                                         &step_motor_handle_id);
   if (res != pdTRUE) {
      link_functions.os_log_info(TAG, "step_motor_task init failed");
   }
#endif

#if FEEDER
   res =
      link_functions.os_thread_create(feeder_task, "feeder_task", 2464, wakeup_cause, (tskIDLE_PRIORITY + 2), &feeder_handle_id);
   if (res != pdTRUE) {
      link_functions.os_log_info(TAG, "feeder_task init failed");
   }
#endif

#if POLISHER
   res = link_functions.os_thread_create(polisher_task, "polisher_task", 2464, wakeup_cause, (tskIDLE_PRIORITY + 2),
                                         &polisher_handle_id);
   if (res != pdTRUE) {
      link_functions.os_log_info(TAG, "polisher_task init failed");
   }
#endif

#if TEST_INT
   res = link_functions.os_thread_create(test_int_task, "test_int_task", 2464, wakeup_cause, (tskIDLE_PRIORITY + 2),
                                         &test_int_handle_id);
   if (res != pdTRUE) {
      link_functions.os_log_info(TAG, "test_int_task init failed");
   }
#endif

#if MORSE
   res = link_functions.os_thread_create(morse_task, "morse_task", 2464, wakeup_cause, (tskIDLE_PRIORITY + 2), &morse_handle_id);
   if (res != pdTRUE) {
      link_functions.os_log_info(TAG, "morse_task init failed");
   }
#endif

#if SCD41_ENABLE
   res = link_functions.os_thread_create(scd41_task, "scd41_task", 2464, wakeup_cause, (tskIDLE_PRIORITY + 2), &scd41_task_handle);
   if (res != pdTRUE) {
      link_functions.os_log_info(TAG, "scd41_task init failed");
   }
#endif

#if EPAPER
   res =
      link_functions.os_thread_create(epaper_task, "epaper_task", 2464, wakeup_cause, (tskIDLE_PRIORITY + 2), &epaper_handle_id);
   if (res != pdTRUE) {
      link_functions.os_log_info(TAG, "epaper_task init failed");
   }
#endif

#if PWM_CONTROL_ENABLE
   res =
      link_functions.os_thread_create(pwm_control_task, "pwm_control_task", 2048, NULL, (tskIDLE_PRIORITY + 2), &pwm_task_handle);
   if (res != pdTRUE) {
      link_functions.os_log_info(TAG, "pwm_control_task init failed");
   }
#endif
}

#endif /* USER_TASKS_INIT_C */
