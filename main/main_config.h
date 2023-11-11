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
#define SLIP_ENABLE 0
#define PWM_TEST_ENABLE 0
#define TOUCH_HANDLE_ENABLE 0
#define UDP_BROADCAST_ENABLE 1
#define UDP_BROADCAST_UDP_REQUEST_ENABLE 1
#define UDP_BROADCAST_INFORMATION_ENABLE 1
#define UDP_ADVERTISMENT_PERIOD 300u
#define MODBUS_MASTER_ENABLE 1
#define DI_HANDLING_ENABLE 0
#define SR04_MODULE 0
#define SS1306_MODULE 0
#define TIME_SYNC_MEASUREMENT_ENABLE 1
#if TIME_SYNC_MEASUREMENT_ENABLE
#define TIME_SYNC_DEVIATION_THRESHOLD 10
#define TIME_SYNC_BUFFER_SIZE 100u
#define TIME_SYNC_EXPEL_BUFFER_SIZE 10u
#define MAX_TRANSMISSION_TIME 1000u
#define DISPLAY_TIME_DIFF 1
#endif
#define STEP_MOTOR 1
#if DI_HANDLING_ENABLE && PMW_TEST_ENABLE
#error "DI_HANDLING_ENABLE and PMW_TEST_ENABLE can't be enabled at the same time"
#endif
#define MAIN_CONFIG_WIFI_AP 0
#define MAIN_CONFIG_WIFI_NODE 0
#define ENABLE_DEEP_SLEEP 1

/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //MAIN_CONFIG_H
