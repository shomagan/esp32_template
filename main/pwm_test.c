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
#endif //CONFIG_IDF_TARGET_ESP32
#include "common.h"
#include "pin_map.h"
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
    pwm_config.cmpr_a = regs_global_part1.vars.test_pwm_value;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = regs_global_part1.vars.test_pwm_value;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
    pwm_test_set(regs_global_part1.vars.test_pwm_value);
#elif (PWM_STEP_CONTROL_ENABLE && CONFIG_IDF_TARGET_ESP32)
    mcpwm_config_t pwm_config = {0};
    pwm_config.frequency = 50;    //frequency = 50Hz,
    pwm_config.cmpr_a = servo_control_part.vars.servo_0;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = servo_control_part.vars.servo_1;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
    pwm_config.frequency = 50;    //frequency = 50Hz,
    pwm_config.cmpr_a = servo_control_part.vars.servo_2;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = servo_control_part.vars.servo_3;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_1, &pwm_config);    //Configure PWM0A & PWM0B with above settings
#endif

    return 0;
}
void pwm_control_task(void *arg){
    (void)arg;
#if (PWM_STEP_CONTROL_ENABLE && CONFIG_IDF_TARGET_ESP32)
    float servo0 = 0.0f;
    float servo1 = 0.0f;
    float servo2 = 0.0f;
    float servo3 = 0.0f;
#endif //PWM_STEP_CONTROL_ENABLE && CONFIG_IDF_TARGET_ESP32    
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
        if (!compare_float_value(servo0, servo_control_part.vars.servo_0, 0.01f)){
            servo0=servo_control_part.vars.servo_0;
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, servo0);
            mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
        }
        if (!compare_float_value(servo1, servo_control_part.vars.servo_1, 0.01f)){
            servo1=servo_control_part.vars.servo_1;
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, servo1);
            mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
        }
        if (!compare_float_value(servo2, servo_control_part.vars.servo_2, 0.01f)){
            servo2=servo_control_part.vars.servo_2;
            mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_A, servo2);
            mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
        }
        if (!compare_float_value(servo3, servo_control_part.vars.servo_3, 0.01f)){
            servo3=servo_control_part.vars.servo_3;
            mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_B, servo3);
            mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
        }
#endif
    }
}

#endif //PWM_TEST_C
