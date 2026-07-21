/**
 * @file pwm_test.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @defgroup ../main/
 * @ingroup ../main/
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
/*
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the sofi PLC.
 *
 * Author: Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 */
#include "os_type.h"
#ifndef PWM_TEST_C
#define PWM_TEST_C 1
#include "pwm_test.h"
#include "regs.h"
#include "regs_description.h"
#if CONFIG_IDF_TARGET_ESP32
#include "driver/mcpwm_prelude.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "driver/rmt_tx.h"
#include "driver/rmt_encoder.h"
#include "driver/rmt_types.h"
#endif
#include "common.h"
#include "pin_map.h"
#include "esp_log.h"
#include "esp_check.h"
#include "link_functions.h"

#if CONFIG_IDF_TARGET_ESP32
#define PWM_TEST_MCPWM_RESOLUTION_HZ (10000000u)
#elif CONFIG_IDF_TARGET_ESP32C3
#define PWM_TEST_RMT_RESOLUTION_HZ (1000000u)
#define SERVO_FRAME_PERIOD_US (20000u)
#define SERVO_PULSE_MIN_US (1000u)
#define SERVO_PULSE_MAX_US (2000u)
#endif

static const char *TAG = "pwm_control";
task_handle_t pwm_task_handle;
static int pwm_test_init(void);
static const u32 def_table_version = 0x0001;
static const float def_servo_0 = 0.0f;
static const float def_min_servo_0 = 0.0f;
static const float def_max_servo_0 = 100.0f;
static const float def_servo_1 = 0.0f;
static const float def_min_servo_1 = 0.0f;
static const float def_max_servo_1 = 100.0f;
static const float def_servo_2 = 0.0f;
static const float def_min_servo_2 = 0.0f;
static const float def_max_servo_2 = 100.0f;
static const float def_servo_3 = 0.0f;
static const float def_min_servo_3 = 0.0f;
static const float def_max_servo_3 = 100.0f;
static const float def_test_pwm_value = 0.0f;
static const float def_min_test_pwm_value = 0.0f;
static const float def_max_test_pwm_value = 100.0f;

/*#generator_use_description {"user_task_regs":"start_struct"}*/
static servo_control_part_t servo_control_part_storage = {{0}};
servo_control_part_t * const servo_control_part = &servo_control_part_storage;
#define NUM_OF_SERVO_CONTROL_PART_VARS 6
static u8 servo_control_part_saved_buf[12];
static const char servo_control_part_space_name[] = "servo_control_p";
static regs_description_t const regs_description_servo_control_part[NUM_OF_SERVO_CONTROL_PART_VARS] = {
    { &def_servo_0, &def_min_servo_0, &def_max_servo_0, (u8*)&servo_control_part->vars.servo_0, 0,"servo pwm value [0;100]","servo_0", 0x14004000, 0x307d0, FLOAT_REGS_FLAG, 1, 193, 2 }//!<"servo pwm value [0;100]" &def &min &max
,
    { &def_servo_1, &def_min_servo_1, &def_max_servo_1, (u8*)&servo_control_part->vars.servo_1, 0,"servo pwm value [0;100]","servo_1", 0x14004001, 0x307d2, FLOAT_REGS_FLAG, 1, 193, 2 }//!<"servo pwm value [0;100]" &def &min &max
,
    { &def_servo_2, &def_min_servo_2, &def_max_servo_2, (u8*)&servo_control_part->vars.servo_2, 0,"servo pwm value [0;100]","servo_2", 0x14004002, 0x307d4, FLOAT_REGS_FLAG, 1, 193, 2 }//!<"servo pwm value [0;100]" &def &min &max
,
    { &def_servo_3, &def_min_servo_3, &def_max_servo_3, (u8*)&servo_control_part->vars.servo_3, 0,"servo pwm value [0;100]","servo_3", 0x14004003, 0x307d6, FLOAT_REGS_FLAG, 1, 193, 2 }//!<"servo pwm value [0;100]" &def &min &max
,
    { &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&servo_control_part->vars.test_pwm_value, 0,"test pwm value [0;100]","test_pwm_value", 0x14004004, 0x307d8, FLOAT_REGS_FLAG, 1, 197, 2 }//!<"test pwm value [0;100]" &def &min &max &save
,
    { &def_table_version, NULL, NULL, (u8*)&servo_control_part->vars.table_version, 4,"table version, resets regs to defaults on mismatch","table_version", 0x14004005, 0x307da, U32_REGS_FLAG, 1, 7, 2 }//!< "table version, resets regs to defaults on mismatch" &ro &save &def
,
};
const regs_description_list_t regs_table_servo_control_part = {
    .description = regs_description_servo_control_part,
    .num_of_regs = NUM_OF_SERVO_CONTROL_PART_VARS,
    .table_version = &def_table_version,
    .space_name = servo_control_part_space_name,
    .saved_regs_buffer = servo_control_part_saved_buf,
    .saved_regs_buffer_size = sizeof(servo_control_part_saved_buf),
};
/*#generator_use_description {"user_task_regs":"end_struct"}*/

#if CONFIG_IDF_TARGET_ESP32

typedef struct {
   mcpwm_timer_handle_t timer;
   mcpwm_oper_handle_t oper;
   mcpwm_cmpr_handle_t comparator_a;
   mcpwm_cmpr_handle_t comparator_b;
   mcpwm_gen_handle_t generator_a;
   mcpwm_gen_handle_t generator_b;
   uint32_t period_ticks;
} pwm_test_pair_t;

static pwm_test_pair_t pwm_air_pair = { 0 };
static pwm_test_pair_t pwm_step_pair0 = { 0 };
static pwm_test_pair_t pwm_step_pair1 = { 0 };

static uint32_t pwm_test_duty_to_ticks(uint32_t period_ticks, float duty_cycle) {
   if (duty_cycle <= 0.0f) {
      return 0;
   }
   if (duty_cycle >= 100.0f) {
      return period_ticks;
   }
   return (uint32_t)((duty_cycle * (float)period_ticks) / 100.0f);
}

static void pwm_test_set_pair(pwm_test_pair_t *pair, float duty_a, float duty_b) {
   ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(pair->comparator_a, pwm_test_duty_to_ticks(pair->period_ticks, duty_a)));
   ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(pair->comparator_b, pwm_test_duty_to_ticks(pair->period_ticks, duty_b)));
}

static void pwm_test_init_pair(pwm_test_pair_t *pair, int group_id, int gpio_a, int gpio_b, uint32_t frequency_hz, float duty_a,
                               float duty_b) {
   pair->period_ticks = PWM_TEST_MCPWM_RESOLUTION_HZ / frequency_hz;

   mcpwm_timer_config_t timer_config = {
      .group_id = group_id,
      .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
      .resolution_hz = PWM_TEST_MCPWM_RESOLUTION_HZ,
      .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
      .period_ticks = pair->period_ticks,
   };
   ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &pair->timer));

   mcpwm_operator_config_t operator_config = {
      .group_id = group_id,
   };
   ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &pair->oper));
   ESP_ERROR_CHECK(mcpwm_operator_connect_timer(pair->oper, pair->timer));

   mcpwm_comparator_config_t comparator_config = {};
   ESP_ERROR_CHECK(mcpwm_new_comparator(pair->oper, &comparator_config, &pair->comparator_a));
   ESP_ERROR_CHECK(mcpwm_new_comparator(pair->oper, &comparator_config, &pair->comparator_b));

   mcpwm_generator_config_t generator_a_config = {
      .gen_gpio_num = gpio_a,
   };
   mcpwm_generator_config_t generator_b_config = {
      .gen_gpio_num = gpio_b,
   };
   ESP_ERROR_CHECK(mcpwm_new_generator(pair->oper, &generator_a_config, &pair->generator_a));
   ESP_ERROR_CHECK(mcpwm_new_generator(pair->oper, &generator_b_config, &pair->generator_b));

   ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
      pair->generator_a, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
   ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
      pair->generator_a, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, pair->comparator_a, MCPWM_GEN_ACTION_LOW)));
   ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
      pair->generator_b, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
   ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
      pair->generator_b, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, pair->comparator_b, MCPWM_GEN_ACTION_LOW)));

   pwm_test_set_pair(pair, duty_a, duty_b);
   ESP_ERROR_CHECK(mcpwm_timer_enable(pair->timer));
   ESP_ERROR_CHECK(mcpwm_timer_start_stop(pair->timer, MCPWM_TIMER_START_NO_STOP));
}
#endif

void pwm_test_set(float duty_cycle) {
#if (PWM_AIR_ENABLE && CONFIG_IDF_TARGET_ESP32)
   pwm_test_set_pair(&pwm_air_pair, duty_cycle, duty_cycle);
#endif //PWM_AIR_ENABLE && CONFIG_IDF_TARGET_ESP32
}

static int pwm_test_init(void) {
   int table_ind = link_functions.regs_description_list_add_new(regs_table_servo_control_part);
   link_functions.preinit_table_vars((u16)table_ind);
#if (PWM_AIR_ENABLE && CONFIG_IDF_TARGET_ESP32)
   pwm_test_init_pair(&pwm_air_pair, 0, GPIO_PWM0A_OUT, GPIO_PWM0B_OUT, 22000u, servo_control_part->vars.test_pwm_value,
                      servo_control_part->vars.test_pwm_value);
   pwm_test_set(servo_control_part->vars.test_pwm_value);
#elif (PWM_STEP_CONTROL_ENABLE && CONFIG_IDF_TARGET_ESP32)
   pwm_test_init_pair(&pwm_step_pair0, 0, GPIO_STEP0_OUT, GPIO_STEP1_OUT, 50u, servo_control_part->vars.servo_0,
                      servo_control_part->vars.servo_1);
   pwm_test_init_pair(&pwm_step_pair1, 1, GPIO_STEP2_OUT, GPIO_STEP3_OUT, 50u, servo_control_part->vars.servo_2,
                      servo_control_part->vars.servo_3);
#endif

   return 0;
}
void pwm_control_task(void *arg) {
   (void)arg;
   uint64_t counter = 0u;
#if PWM_STEP_CONTROL_ENABLE
#if CONFIG_IDF_TARGET_ESP32
   float servo0 = 0.0f;
   float servo1 = 0.0f;
   float servo2 = 0.0f;
   float servo3 = 0.0f;
#endif
#endif
#if (PWM_CONTROL_ENABLE && CONFIG_IDF_TARGET_ESP32C3)
   rmt_symbol_word_t rmt_items_loop = { 0 };
   rmt_channel_handle_t tx_chan = NULL;
   rmt_encoder_handle_t copy_encoder = NULL;
   rmt_tx_channel_config_t tx_chan_cfg = {
      .gpio_num = GPIO_PWM_TEST_OUT,
      .clk_src = RMT_CLK_SRC_DEFAULT,
      .resolution_hz = PWM_TEST_RMT_RESOLUTION_HZ,
      .mem_block_symbols = 64,
      .trans_queue_depth = 2,
      .flags.invert_out = false,
      .flags.with_dma = false,
   };
   ESP_RETURN_VOID_ON_ERROR(rmt_new_tx_channel(&tx_chan_cfg, &tx_chan), TAG, "Failed to create RMT TX channel");
   rmt_copy_encoder_config_t encoder_cfg = {};
   ESP_RETURN_VOID_ON_ERROR(rmt_new_copy_encoder(&encoder_cfg, &copy_encoder), TAG, "Failed to create RMT encoder");
   ESP_RETURN_VOID_ON_ERROR(rmt_enable(tx_chan), TAG, "Failed to enable RMT channel");
   uint16_t pwm_value_last = 0xffffu; /*force initial transmit so the ESC sees a valid frame from startup*/
#endif
   uint32_t signal_value;
   pwm_test_init();
#if (PWM_STEP_CONTROL_ENABLE && CONFIG_IDF_TARGET_ESP32)
   servo0 = servo_control_part->vars.servo_0;
   servo1 = servo_control_part->vars.servo_1;
   servo2 = servo_control_part->vars.servo_2;
   servo3 = servo_control_part->vars.servo_3;
#endif
   while (1) {
      if (link_functions.os_thread_signal_wait(STOP_CHILD_PROCCES | PACKET_RECEIVED, &signal_value, 100) == pdTRUE) {
         /*by signal*/
         if (signal_value & STOP_CHILD_PROCCES) {
         } else if (signal_value & PACKET_RECEIVED) {
         }
      }
#if (PWM_STEP_CONTROL_ENABLE && CONFIG_IDF_TARGET_ESP32)
      float next_servo0 = servo_control_part->vars.servo_0;
      float next_servo1 = servo_control_part->vars.servo_1;
      float next_servo2 = servo_control_part->vars.servo_2;
      float next_servo3 = servo_control_part->vars.servo_3;
      if (!compare_float_value(servo0, next_servo0, 0.01f) || !compare_float_value(servo1, next_servo1, 0.01f)) {
         servo0 = next_servo0;
         servo1 = next_servo1;
         pwm_test_set_pair(&pwm_step_pair0, servo0, servo1);
      }
      if (!compare_float_value(servo2, next_servo2, 0.01f) || !compare_float_value(servo3, next_servo3, 0.01f)) {
         servo2 = next_servo2;
         servo3 = next_servo3;
         pwm_test_set_pair(&pwm_step_pair1, servo2, servo3);
      }
#endif
#if (PWM_CONTROL_ENABLE && CONFIG_IDF_TARGET_ESP32C3)
      uint16_t pwm_value;
      if(counter < 20){
         pwm_value = 0;
      }else{
         pwm_value = (uint16_t)servo_control_part->vars.test_pwm_value;
      }

      if (counter % 10 == 0) {
         main_printf(TAG, "PWM Test value: %d", pwm_value);
      }
      if (pwm_value != pwm_value_last) {
         pwm_value_last = pwm_value;
         ESP_ERROR_CHECK(rmt_disable(tx_chan));
         ESP_ERROR_CHECK(rmt_enable(tx_chan));
         uint32_t pulse_us = SERVO_PULSE_MIN_US + ((uint32_t)pwm_value * (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US)) / 100u;
         rmt_items_loop.duration1 = pulse_us;
         rmt_items_loop.level1 = 1;
         rmt_items_loop.level0 = 0;
         rmt_items_loop.duration0 = SERVO_FRAME_PERIOD_US - pulse_us;
         rmt_transmit_config_t tx_cfg = {
            .loop_count = -1,
         };
         ESP_ERROR_CHECK(rmt_transmit(tx_chan, copy_encoder, &rmt_items_loop, sizeof(rmt_items_loop), &tx_cfg));
      }
#endif
      counter++;
   }
}
#endif //PWM_TEST_C
