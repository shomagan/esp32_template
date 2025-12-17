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
 * Copyright (c) 2018 Snema Service
 * All rights reserved.
 *
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
#ifndef PWM_TEST_C
#define PWM_TEST_C 1
#include "pwm_test.h"
#include "regs.h"
#include "regs_description.h"
#include "driver/mcpwm.h"
#if CONFIG_IDF_TARGET_ESP32
#include "soc/mcpwm_periph.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "driver/rmt.h"
#include "hal/rmt_types.h"
#endif
#include "common.h"
#include "pin_map.h"
#include "esp_log.h"
#include "esp_check.h"
static const char *TAG = "pwm_control";
static IRAM_ATTR void rmt_tx_loop_intr(rmt_channel_t channel, void *args);
static void mcpwm_example_gpio_initialize(void);
task_handle_t pwm_task_handle;
static int pwm_test_init(void);
static void mcpwm_example_gpio_initialize(void){
#if PWM_AIR_ENABLE
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_PWM0A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, GPIO_PWM0B_OUT);
#elif PWM_STEP_CONTROL_ENABLE
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_STEP0_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, GPIO_STEP1_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1A, GPIO_STEP2_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1B, GPIO_STEP3_OUT);
#endif
}
void pwm_test_set(float duty_cycle){
#if (PWM_AIR_ENABLE && CONFIG_IDF_TARGET_ESP32)
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_cycle);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, duty_cycle);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
#endif //PWM_AIR_ENABLE && CONFIG_IDF_TARGET_ESP32
}

static int pwm_test_init(void){
    mcpwm_example_gpio_initialize();
#if (PWM_AIR_ENABLE && CONFIG_IDF_TARGET_ESP32)
    mcpwm_config_t pwm_config = {0};
    pwm_config.frequency = 22000;    //frequency =22000Hz,
    pwm_config.cmpr_a = regs_global_part1->vars.test_pwm_value;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = regs_global_part1->vars.test_pwm_value;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
    pwm_test_set(regs_global_part1->vars.test_pwm_value);
#elif (PWM_STEP_CONTROL_ENABLE && CONFIG_IDF_TARGET_ESP32)
    mcpwm_config_t pwm_config = {0};
    pwm_config.frequency = 50;    //frequency = 50Hz,
    pwm_config.cmpr_a = servo_control_part->vars.servo_0;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = servo_control_part->vars.servo_1;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
    pwm_config.frequency = 50;    //frequency = 50Hz,
    pwm_config.cmpr_a = servo_control_part->vars.servo_2;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = servo_control_part->vars.servo_3;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_1, &pwm_config);    //Configure PWM0A & PWM0B with above settings
#endif

    return 0;
}
void pwm_control_task(void *arg){
    (void)arg;
    uint32_t counter = 0u;
#if PWM_STEP_CONTROL_ENABLE 
#if CONFIG_IDF_TARGET_ESP32
    float servo0 = 0.0f;
    float servo1 = 0.0f;
    float servo2 = 0.0f;
    float servo3 = 0.0f;
#endif
#endif
#if (PWM_CONTROL_ENABLE && CONFIG_IDF_TARGET_ESP32C3)
   rmt_item32_t rmt_items_loop;
   rmt_config_t dev_config = RMT_DEFAULT_CONFIG_TX(GPIO_OUTPUT_STEP_MOTOR_STEP0, RMT_CHANNEL_0);
   dev_config.tx_config.carrier_freq_hz = 28000;
   dev_config.tx_config.carrier_duty_percent = 0;
   dev_config.tx_config.carrier_en = false;
   dev_config.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
   dev_config.tx_config.loop_en = true;
   ESP_RETURN_ON_ERROR(rmt_config(&dev_config), TAG, "Failed to configure RMT");
   ESP_RETURN_ON_ERROR(rmt_driver_install(dev_config.channel, 0, 0), TAG, "Failed to install RMT driver");
   // register tx end callback function, which got invoked when tx loop comes to the end
   rmt_register_tx_end_callback(rmt_tx_loop_intr, &dev_config);
   uint16_t pwm_value_last = 0;
#endif 
    uint32_t signal_value;
    pwm_test_init();
    while(1){
        if(task_notify_wait(STOP_CHILD_PROCCES|PACKET_RECEIVED, &signal_value, 100)==pdTRUE){
            /*by signal*/
            if (signal_value & STOP_CHILD_PROCCES){
            }else if(signal_value & PACKET_RECEIVED){
            }
        }
#if (PWM_STEP_CONTROL_ENABLE && CONFIG_IDF_TARGET_ESP32)
        if (!compare_float_value(servo0, servo_control_part->vars.servo_0, 0.01f)){
            servo0=servo_control_part->vars.servo_0;
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, servo0);
            mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
        }
        if (!compare_float_value(servo1, servo_control_part->vars.servo_1, 0.01f)){
            servo1=servo_control_part->vars.servo_1;
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, servo1);
            mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
        }
        if (!compare_float_value(servo2, servo_control_part->vars.servo_2, 0.01f)){
            servo2=servo_control_part->vars.servo_2;
            mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_A, servo2);
            mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
        }
        if (!compare_float_value(servo3, servo_control_part->vars.servo_3, 0.01f)){
            servo3=servo_control_part->vars.servo_3;
            mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_B, servo3);
            mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
        }
#endif
#if (PWM_CONTROL_ENABLE && CONFIG_IDF_TARGET_ESP32C3)
         uint16_t pwm_value = (uint16_t)regs_global_part1->vars.test_pwm_value;
         if(counter % 10 == 0) {
               main_printf(TAG, "PWM Test value: %d", pwm_value);
         }
         if (pwm_value != pwm_value_last) {
            pwm_value_last = pwm_value;
            rmt_tx_stop(dev_config.channel);
            if (pwm_value > 1) {
               rmt_set_tx_loop_count(dev_config.channel, 0);
               rmt_enable_tx_loop_autostop(dev_config.channel, false);
               rmt_set_tx_loop_mode(dev_config.channel, true);
               rmt_items_loop.duration1 = pwm_value;
               rmt_items_loop.level1 = 1;
               rmt_items_loop.level0 = 0;
               rmt_items_loop.duration0 = 100 - pwm_value;
               ESP_ERROR_CHECK(rmt_write_items(dev_config.channel, &rmt_items_loop, 1, false));
            } 
         }
#endif         
         counter++;
    }
}

static IRAM_ATTR void rmt_tx_loop_intr(rmt_channel_t channel, void *args)
{
   rmt_config_t *dev_config = (rmt_config_t *)args;

}
#endif //PWM_TEST_C
