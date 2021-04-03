/**
 * @file common.h
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
#ifndef COMMON_H
#define COMMON_H 1
 
/*add includes below */
#include "type_def.h"
#include "os_type.h"
/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
#define TIME_FOR_PREPARE_RESET_MS 2000
/*add functions and variable declarations below */

   enum dinamic_task_event{
       STOP_CHILD_PROCCES          = (1<<0),   /*!< sended before kill external thread*/
       STOP_PROCESS_FOR_CHANGE_OS  = (1<<1),   /*!< sended before change os*/
       CAN_RX_EVENT                = (1<<2),   /*!< when can rx event */
       CAN_TX_EVENT                = (1<<3),   /*!< when can tx event */
       CAN_TIMER_EVENT             = (1<<4),   /*!< when can timer event */
       CAN_DICT_DEFAULT            = (1<<5),   /*!< return can dict to default*/
       PACKET_MAIL_SENDED          = (1<<6),   /*!< send mail through PM*/
       PACKET_RECEIVED             = (1<<7),   /*!< receved packet for PM*/
       SDO_TRANSMITHING            = (1<<8),   /*!< start transmith sdo parametr*/
       ETHERNET_RX_EVENT           = (1<<9),   /*!< we have received rx over ethernet*/
       SLIP_RX_EVENT               = (1<<10),   /*!< we have received rx over SLIP*/
       HARDWARE_DEINIT_EVENT       = (1<<11),   /*!< we have problem with hardware*/
       HAVE_DATA_TO_SEND           = (1<<12),   /*!< we have data to send*/
       SAVE_BKRAM_TO_FLASH         = (1<<13),   /*!< save bkram to flash command*/
       ARC_REMOVE_ALL              = (1<<14),   /*!< remove all arc in flash*/
       LOG_WRITE                   = (1<<15),   /*!< save log temp file to flash*/
       PACKET_TRANSMITION_CLOSE    = (1<<16),   /*!< sent last byte */
       WIFI_CONNECTED              = (1<<17),   /*!< wifi connected*/
       PREPARE_TO_RESET            = (1<<18),   /*!< controller will reset througth TIME_FOR_PREPARE_RESET_MS*/
       //.............................................................................
       UNUSED_EVENT_1              = (1<<28),   /*!< we should not use this bit!!*/
       RAM_ADDRESS_IN_EVENT        = (1<<29),   /*!< instead event use ram address*/
       UNUSED_EVENT_2              = (1<<30),   /*!< we should not use this bit!!*/
       FLASH_ADDRESS_IN_EVENT      = (1<<31),   /*!< instead event use flash address*/
   };

extern task_handle_t common_duty_task_handle;

/**
 * @brief led_blink_on enable the led on ms
 * @param time_ms
 */
void led_blink_on(u16 time_ms);
/**
 * @brief led_blink_off disable the led if it was enableb
 */
void led_blink_off(void);
/**
 * @brief common_init- init the common(system, like LED)
 * @return
 */
int common_init(void);
/**
 * @brief common_deinit
 * @return
 */
int common_deinit(void);
/**
 * @brief common_init_tasks - all task must starting here
 * @return
 */
int common_init_tasks(void);
/**
 * @brief is_ascii_symbol_or_digital
 * @param buff
 * @param len must be more then 0
 * @return 1 if consist if symbol or digital
 */
int is_ascii_symbol_or_digital(u8 * buff, u32 len);
/**
 * @brief memcmp_self
 * @param buff_0
 * @param buff_1
 * @param len
 * @return 0 -if not equal, 1 - if equal
 */
int memcmp_not_equal(const u8 * buff_0,const u8 * buff_1,u32 len);

/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //COMMON_H
