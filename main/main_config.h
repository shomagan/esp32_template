/**
 * @file main_config.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @defgroup ../main/
 * @ingroup ../main/
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
/*
 * All rights not reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are not permitted and the following conditions are not met:
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
 * This file is part of the sofi concept.
 *
 * Author: Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 */
#ifndef MAIN_CONFIG_H
#define MAIN_CONFIG_H 1
 
/*add includes below */
#include "type_def.h"

/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */
#ifndef RELEASE
#define RELEASE 1
#endif
#define INTERNAL_FLASH_MIRROR_ITEM_SIZE 512u
#ifndef MAIN_DEBUG_NOASSERT
#define MAIN_DEBUG_ASSERT(message, assertion) do { if (!(assertion)) { \
  ESP_ERROR_CHECK(message); }} while(0)
#else
#define MAIN_DEBUG_ASSERT(message, assertion)
#endif  //main_noassert
/*task config start */
#define ENABLE_DEEP_SLEEP 1
#define SLIP_ENABLE 0
#define PWM_TEST_ENABLE 0
#define TOUCH_HANDLE_ENABLE 0
#define UDP_BROADCAST_ENABLE 1
#define UDP_BROADCAST_UDP_REQUEST_ENABLE 1
#define UDP_BROADCAST_INFORMATION_ENABLE 1
#define UDP_ADVERTISMENT_PERIOD 1000u
#define MODBUS_MASTER_ENABLE 0
#define DI_HANDLING_ENABLE 0
#define SR04_MODULE 0
#define DISPLAY 0
#define TIME_SYNC_MEASUREMENT_ENABLE 1
#define STEP_MOTOR 0
#define FEEDER 0
#define POLISHER 0
#define TEST_INT 0
#define MORSE 0
#define BATTERY_STATE 0
#define TELEGRAM 1
#define SCD41_ENABLE 1
#if TIME_SYNC_MEASUREMENT_ENABLE
#define TIME_SYNC_DEVIATION_THRESHOLD 10
#define TIME_SYNC_BUFFER_SIZE 100u
#define TIME_SYNC_EXPEL_BUFFER_SIZE 10u
#define MAX_TRANSMISSION_TIME 1000u
#define DISPLAY_TIME_DIFF 0
#endif
#if DI_HANDLING_ENABLE && PMW_TEST_ENABLE
#error "DI_HANDLING_ENABLE and PMW_TEST_ENABLE can't be enabled at the same time"
#endif
#if (FEEDER && STEP_MOTOR)|| (POLISHER && STEP_MOTOR)|| (FEEDER && POLISHER)
  #error "config problem"
#endif
#if (SCD41_ENABLE && DISPLAY)
  #error "config problem"
#endif
#if MORSE
#define DEVICE_NAME "morse"
#elif FEEDER
#define DEVICE_NAME "feeder"
#elif POLISHER
#define DEVICE_NAME "polisher"
#elif TEST_INT
#define DEVICE_NAME "test_int"
#elif STEP_MOTOR
#define DEVICE_NAME "step_motor"
#elif DI_HANDLING_ENABLE
#define DEVICE_NAME "di_handling"
#elif SCD41_ENABLE
#define DEVICE_NAME "scd41"
#endif

/*task config end */


#define MAIN_CONFIG_WIFI_AP 0
#define MAIN_CONFIG_WIFI_NODE 0


/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //MAIN_CONFIG_H
