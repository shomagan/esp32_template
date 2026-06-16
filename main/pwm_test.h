/**
 * @file pwm_test.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @defgroup ../main/
 * @ingroup ../main/
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
/*
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
#ifndef PWM_TEST_H
#define PWM_TEST_H 1

/*add includes below */
#include "main_config.h"
#include "type_def.h"
#include "os_type.h"


/*add includes before */
#ifdef __cplusplus
   extern "C" {
#endif

/*add functions and variable declarations below */

#define PWM_STEP_CONTROL_ENABLE 0
#define PWM_AIR_ENABLE 0

/**
 * @brief main struct
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  &crtcl- restart after change value, \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"servo_control_part_t",  "address_space" :2, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :2000}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        float servo_0;                   //!<"servo pwm value [0;100]" &def &min &max
        float servo_1;                   //!<"servo pwm value [0;100]" &def &min &max
        float servo_2;                   //!<"servo pwm value [0;100]" &def &min &max
        float servo_3;                   //!<"servo pwm value [0;100]" &def &min &max
    }vars;
    u8 bytes[32]; //for full bksram copy
}servo_control_part_t;// #generator_use_description {"message":"end_struct"}
extern servo_control_part_t * const servo_control_part;

extern task_handle_t pwm_task_handle;
void pwm_test_set(float duty_cycle);

void pwm_control_task(void *arg);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //PWM_TEST_H
