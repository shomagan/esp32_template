/**
 * @file touch_handle.h
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
#ifndef TOUCH_HANDLE_H
#define TOUCH_HANDLE_H 1

/*add includes below */
#include "type_def.h"
#include "os_type.h"


/*add includes before */
#ifdef __cplusplus
   extern "C" {
#endif
/*add functions and variable declarations below */
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
/** #generator_use_description {"space_name" :"touch_regs_t", "modbus_type" :"server", "address_space" :1, "modbus_function" :"holding_registers", "register_start_address" :1000}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        float test_pwm_value;                   //!<"test pwm value [0;100]" &def &save &min &max
        u16 touch_0;
        u16 touch_1;
        u16 touch_2;
        u16 touch_3;
        u64 water_counter;                      //!< "di counter" &save &ro
        float impulse_cost;                     //!< "cost of di impulse in liters" &save &def
        float liters;                           //!< "liters calculated" &save
        u32 touch_1_count;                      //!< "ms " &save &def
        u32 touch_2_count;                      //!< "ms " &save &def
        u32 touch_3_count;                      //!< "ms " &save &def
        float touch_1_liters;                      //!< "ms " &save &def
        float touch_2_liters;                      //!< "ms " &save &def
        float touch_3_liters;                      //!< "ms " &save &def
        float touch_0_trshld;                      //!< "in percents" &save &def
        float touch_1_trshld;                      //!< "in percents" &save &def
        float touch_2_trshld;                      //!< "in percents" &save &def
        float touch_3_trshld;                      //!< "in percents" &save &def
        u32 filter_use;                             //!< "use inside filter fot touch sensors or not " &save
        u32 touch_handle_period;                    //!< "in ms period of handle touchs" &save &def
        u32 by_time;                              //!< "by time or counter" &save &def
    }vars;
    u8 bytes[32]; //for full bksram copy
}touch_regs_t;// #generator_use_description {"message":"end_struct"}
extern touch_regs_t * const touch_regs;

extern task_handle_t touch_task_handle;
void touch_task(void *arg);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //TOUCH_HANDLE
