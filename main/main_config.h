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
#define INTERNAL_FLASH_MIRROR_ITEM_SIZE 512
#ifndef MAIN_DEBUG_NOASSERT
#define MAIN_DEBUG_ASSERT(message, assertion) do { if (!(assertion)) { \
  ESP_ERROR_CHECK(message); }} while(0)
#else
#define MAIN_DEBUG_ASSERT(message, assertion)
#endif  //main_noassert
#define PWM_TEST_ENABLE 1
#define TOUCH_HANDLE_ENABLE 1
#define UDP_BROADCAST_ENABLE 1
#define UDP_ADVERTISMENT_PERIOD 1000
#define MODBUS_MASTER_ENABLE 1
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //MAIN_CONFIG_H
